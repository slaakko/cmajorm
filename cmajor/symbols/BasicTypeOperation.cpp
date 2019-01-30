// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/BasicTypeOperation.hpp>

namespace cmajor { namespace symbols {

BasicTypeUnaryPlusOperation::BasicTypeUnaryPlusOperation(const Span& span_, const std::u32string& name_) : BasicTypeUnaryOperation<BasicTypeUnaryPlus>(SymbolType::basicTypeUnaryPlus)
{
}

BasicTypeIntUnaryMinusOperation::BasicTypeIntUnaryMinusOperation(const Span& span_, const std::u32string& name_) : BasicTypeUnaryOperation<BasicTypeIntUnaryMinus>(SymbolType::basicTypeIntUnaryMinus)
{
}

BasicTypeFloatUnaryMinusOperation::BasicTypeFloatUnaryMinusOperation(const Span& span_, const std::u32string& name_) : BasicTypeUnaryOperation<BasicTypeFloatUnaryMinus>(SymbolType::basicTypeFloatUnaryMinus)
{
}

BasicTypeComplementOperation::BasicTypeComplementOperation(const Span& span_, const std::u32string& name_) : BasicTypeUnaryOperation<BasicTypeComplement>(SymbolType::basicTypeComplement)
{
}

BasicTypeNotOperation::BasicTypeNotOperation(const Span& span_, const std::u32string& name_) : BasicTypeUnaryOperation<BasicTypeNot>(SymbolType::basicTypeNot)
{
}

BasicTypeAddOperation::BasicTypeAddOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeAdd>(SymbolType::basicTypeAdd)
{
}

BasicTypeFAddOperation::BasicTypeFAddOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeFAdd>(SymbolType::basicTypeFAdd)
{
}

BasicTypeSubOperation::BasicTypeSubOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeSub>(SymbolType::basicTypeSub)
{
}

BasicTypeFSubOperation::BasicTypeFSubOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeFSub>(SymbolType::basicTypeFSub)
{
}

BasicTypeMulOperation::BasicTypeMulOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeMul>(SymbolType::basicTypeMul)
{
}

BasicTypeFMulOperation::BasicTypeFMulOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeFMul>(SymbolType::basicTypeFMul)
{
}

BasicTypeSDivOperation::BasicTypeSDivOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeSDiv>(SymbolType::basicTypeSDiv)
{
}

BasicTypeUDivOperation::BasicTypeUDivOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeUDiv>(SymbolType::basicTypeUDiv)
{
}

BasicTypeFDivOperation::BasicTypeFDivOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeFDiv>(SymbolType::basicTypeFDiv)
{
}

BasicTypeSRemOperation::BasicTypeSRemOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeSRem>(SymbolType::basicTypeSRem)
{
}

BasicTypeURemOperation::BasicTypeURemOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeURem>(SymbolType::basicTypeURem)
{
}

BasicTypeAndOperation::BasicTypeAndOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeAnd>(SymbolType::basicTypeAnd)
{
}

BasicTypeOrOperation::BasicTypeOrOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeOr>(SymbolType::basicTypeOr)
{
}

BasicTypeXorOperation::BasicTypeXorOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeXor>(SymbolType::basicTypeXor)
{
}

BasicTypeShlOperation::BasicTypeShlOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeShl>(SymbolType::basicTypeShl)
{
}

BasicTypeAShrOperation::BasicTypeAShrOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeAShr>(SymbolType::basicTypeAShr)
{
}

BasicTypeLShrOperation::BasicTypeLShrOperation(const Span& span_, const std::u32string& name_) : BasicTypeBinaryOperation<BasicTypeLShr>(SymbolType::basicTypeLShr)
{
}

BasicTypeDefaultInt1Operation::BasicTypeDefaultInt1Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultInt1>(SymbolType::defaultInt1)
{
}

BasicTypeDefaultSInt8Operation::BasicTypeDefaultSInt8Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultSInt8>(SymbolType::defaultSInt8)
{
}

BasicTypeDefaultUInt8Operation::BasicTypeDefaultUInt8Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultUInt8>(SymbolType::defaultUInt8)
{
}

BasicTypeDefaultSInt16Operation::BasicTypeDefaultSInt16Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultSInt16>(SymbolType::defaultSInt16)
{
}

BasicTypeDefaultUInt16Operation::BasicTypeDefaultUInt16Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultUInt16>(SymbolType::defaultUInt16)
{
}

BasicTypeDefaultSInt32Operation::BasicTypeDefaultSInt32Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultSInt32>(SymbolType::defaultSInt32)
{
}

BasicTypeDefaultUInt32Operation::BasicTypeDefaultUInt32Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultUInt32>(SymbolType::defaultUInt32)
{
}

BasicTypeDefaultSInt64Operation::BasicTypeDefaultSInt64Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultSInt64>(SymbolType::defaultSInt64)
{
}

BasicTypeDefaultUInt64Operation::BasicTypeDefaultUInt64Operation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultUInt64>(SymbolType::defaultUInt64)
{
}

BasicTypeDefaultFloatOperation::BasicTypeDefaultFloatOperation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultFloat>(SymbolType::defaultFloat)
{
}

BasicTypeDefaultDoubleOperation::BasicTypeDefaultDoubleOperation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultDouble>(SymbolType::defaultDouble)
{
}

BasicTypeDefaultCharOperation::BasicTypeDefaultCharOperation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultChar>(SymbolType::defaultChar)
{
}

BasicTypeDefaultWCharOperation::BasicTypeDefaultWCharOperation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultWChar>(SymbolType::defaultWChar)
{
}

BasicTypeDefaultUCharOperation::BasicTypeDefaultUCharOperation(const Span& span_, const std::u32string& name_) : BasicTypeDefaultCtor<DefaultUChar>(SymbolType::defaultUChar)
{
}

BasicTypeCopyCtor::BasicTypeCopyCtor(TypeSymbol* type) : FunctionSymbol(SymbolType::basicTypeCopyCtor, Span(), U"@constructor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(type->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    ComputeName();
    if (type->IsBasicTypeSymbol())
    {
        BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(type);
        basicTypeSymbol->SetCopyConstructor(this);
    }
}

BasicTypeCopyCtor::BasicTypeCopyCtor(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::basicTypeCopyCtor, span_, name_)
{
}

void BasicTypeCopyCtor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, flags & OperationFlags::functionCallFlags);
}

std::unique_ptr<Value> BasicTypeCopyCtor::ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span) const
{
    Assert(argumentValues.size() == 1, "one source value expected");
    return std::unique_ptr<Value>(argumentValues[0]->Clone());
}

BasicTypeMoveCtor::BasicTypeMoveCtor(TypeSymbol* type) : FunctionSymbol(SymbolType::basicTypeMoveCtor, Span(), U"@constructor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(type->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(type->AddRvalueReference(Span()));
    AddMember(thatParam);
    ComputeName();
    if (type->IsBasicTypeSymbol())
    {
        BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(type);
        basicTypeSymbol->SetMoveConstructor(this);
    }
}

BasicTypeMoveCtor::BasicTypeMoveCtor(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::basicTypeMoveCtor, span_, name_)
{
}

void BasicTypeMoveCtor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "move constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    genObjects[0]->Store(emitter, flags & OperationFlags::functionCallFlags);
}

std::unique_ptr<Value> BasicTypeMoveCtor::ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span) const
{
    Assert(argumentValues.size() == 1, "one source value expected");
    return std::unique_ptr<Value>(argumentValues[0]->Clone());
}

BasicTypeCopyAssignment::BasicTypeCopyAssignment(TypeSymbol* type, TypeSymbol* voidType) : FunctionSymbol(SymbolType::basicTypeCopyAssignment, Span(), U"operator=")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(type->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    SetReturnType(voidType);
    ComputeName();
    if (type->IsBasicTypeSymbol())
    {
        BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(type);
        basicTypeSymbol->SetCopyAssignment(this);
    }
}

BasicTypeCopyAssignment::BasicTypeCopyAssignment(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::basicTypeCopyAssignment, span_, name_)
{
}

void BasicTypeCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

BasicTypeMoveAssignment::BasicTypeMoveAssignment(TypeSymbol* type, TypeSymbol* voidType) : FunctionSymbol(SymbolType::basicTypeMoveAssignment, Span(), U"operator=")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(type->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(type->AddRvalueReference(Span()));
    AddMember(thatParam);
    SetReturnType(voidType);
    ComputeName();
    if (type->IsBasicTypeSymbol())
    {
        BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(type);
        basicTypeSymbol->SetMoveAssignment(this);
    }
}

BasicTypeMoveAssignment::BasicTypeMoveAssignment(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::basicTypeMoveAssignment, span_, name_)
{
}

void BasicTypeMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "move assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    genObjects[0]->Store(emitter, OperationFlags::none);
}

BasicTypeReturn::BasicTypeReturn(TypeSymbol* type) : FunctionSymbol(SymbolType::basicTypeReturn, Span(), U"@return")
{
    SetGroupName(U"@return");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* valueParam = new ParameterSymbol(Span(), U"value");
    valueParam->SetType(type);
    AddMember(valueParam);
    SetReturnType(type);
    ComputeName();
    if (type->IsBasicTypeSymbol())
    {
        BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(type);
        basicTypeSymbol->SetReturnFun(this);
    }
}

BasicTypeReturn::BasicTypeReturn(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::basicTypeReturn, span_, name_)
{
}

void BasicTypeReturn::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "return needs one object");
    genObjects[0]->Load(emitter, OperationFlags::none);
}

BasicTypeIntegerEqualityOperation::BasicTypeIntegerEqualityOperation(const Span& span_, const std::u32string& name_) : BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality)
{
}

BasicTypeFloatingEqualityOperation::BasicTypeFloatingEqualityOperation(const Span& span_, const std::u32string& name_) : BasicTypeComparisonOperation<BasicTypeFloatingEquality>(SymbolType::basicTypeFloatingEquality)
{
}

BasicTypeUnsignedIntegerLessThanOperation::BasicTypeUnsignedIntegerLessThanOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan)
{
}

BasicTypeSignedIntegerLessThanOperation::BasicTypeSignedIntegerLessThanOperation(const Span& span_, const std::u32string& name_) :
    BasicTypeComparisonOperation<BasicTypeSignedIntegerLessThan>(SymbolType::basicTypeSignedIntegerLessThan)
{
}

BasicTypeFloatingLessThanOperation::BasicTypeFloatingLessThanOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeComparisonOperation<BasicTypeFloatingLessThan>(SymbolType::basicTypeFloatingLessThan)
{
}

BasicTypeImplicitSignExtensionOperation::BasicTypeImplicitSignExtensionOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeImplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeImplicitSignExtension)
{
}

BasicTypeImplicitZeroExtensionOperation::BasicTypeImplicitZeroExtensionOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension)
{
}

BasicTypeExplicitSignExtensionOperation::BasicTypeExplicitSignExtensionOperation(const Span& span_, const std::u32string& name_) :
    BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension)
{
}

BasicTypeExplicitZeroExtensionOperation::BasicTypeExplicitZeroExtensionOperation(const Span& span_, const std::u32string& name_) :
    BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension)
{
}

BasicTypeTruncationOperation::BasicTypeTruncationOperation(const Span& span_, const std::u32string& name_) : BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation)
{
}

BasicTypeBitCastOperation::BasicTypeBitCastOperation(const Span& span_, const std::u32string& name_) : BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast)
{
}

BasicTypeImplicitUnsignedIntToFloatingOperation::BasicTypeImplicitUnsignedIntToFloatingOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating)
{
}

BasicTypeImplicitSignedIntToFloatingOperation::BasicTypeImplicitSignedIntToFloatingOperation(const Span& span_, const std::u32string& name_) :
    BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating)
{
}

BasicTypeExplicitUnsignedIntToFloatingOperation::BasicTypeExplicitUnsignedIntToFloatingOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating)
{
}

BasicTypeExplicitSignedIntToFloatingOperation::BasicTypeExplicitSignedIntToFloatingOperation(const Span& span_, const std::u32string& name_) :
    BasicTypeExplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeExplicitSignedIntToFloating)
{
}

BasicTypeFloatingToUnsignedIntOperation::BasicTypeFloatingToUnsignedIntOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt)
{
}

BasicTypeFloatingToSignedIntOperation::BasicTypeFloatingToSignedIntOperation(const Span& span_, const std::u32string& name_) :
    BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt)
{
}

BasicTypeFloatingExtensionOperation::BasicTypeFloatingExtensionOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeImplicitConversion<BasicTypeFloatingExtension>(SymbolType::basicTypeFloatingExtension)
{
}

BasicTypeFloatingTruncationOperation::BasicTypeFloatingTruncationOperation(const Span& span_, const std::u32string& name_) : 
    BasicTypeExplicitConversion<BasicTypeFloatingTruncation>(SymbolType::basicTypeFloatingTruncation)
{
}

void MakeSignedIntegerTypeOperations(SymbolTable& symbolTable, TypeSymbol* type)
{
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeUnaryPlus>(SymbolType::basicTypeUnaryPlus, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeIntUnaryMinus>(SymbolType::basicTypeIntUnaryMinus, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeComplement>(SymbolType::basicTypeComplement, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeAdd>(SymbolType::basicTypeAdd, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeSub>(SymbolType::basicTypeSub, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeMul>(SymbolType::basicTypeMul, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeSDiv>(SymbolType::basicTypeSDiv, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeSRem>(SymbolType::basicTypeSRem, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeAnd>(SymbolType::basicTypeAnd, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeOr>(SymbolType::basicTypeOr, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeXor>(SymbolType::basicTypeXor, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeShl>(SymbolType::basicTypeShl, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeAShr>(SymbolType::basicTypeAShr, type));
}

void MakeUnsignedIntegerTypeOperations(SymbolTable& symbolTable, TypeSymbol* type)
{
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeUnaryPlus>(SymbolType::basicTypeUnaryPlus, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeIntUnaryMinus>(SymbolType::basicTypeIntUnaryMinus, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeComplement>(SymbolType::basicTypeComplement, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeAdd>(SymbolType::basicTypeAdd, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeSub>(SymbolType::basicTypeSub, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeMul>(SymbolType::basicTypeMul, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeUDiv>(SymbolType::basicTypeUDiv, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeURem>(SymbolType::basicTypeURem, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeAnd>(SymbolType::basicTypeAnd, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeOr>(SymbolType::basicTypeOr, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeXor>(SymbolType::basicTypeXor, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeShl>(SymbolType::basicTypeShl, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeLShr>(SymbolType::basicTypeLShr, type));
}

void MakeFloatingPointTypeOperations(SymbolTable& symbolTable, TypeSymbol* type)
{
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeUnaryPlus>(SymbolType::basicTypeUnaryPlus, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeFloatUnaryMinus>(SymbolType::basicTypeFloatUnaryMinus, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeFAdd>(SymbolType::basicTypeFAdd, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeFSub>(SymbolType::basicTypeFSub, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeFMul>(SymbolType::basicTypeFMul, type));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeBinaryOperation<BasicTypeFDiv>(SymbolType::basicTypeFDiv, type));
}

void MakeBasicTypeConversions(SymbolTable& symbolTable, BoolTypeSymbol* boolType, SByteTypeSymbol* sbyteType, ByteTypeSymbol* byteType, ShortTypeSymbol* shortType, UShortTypeSymbol* ushortType,
    IntTypeSymbol* intType, UIntTypeSymbol* uintType, LongTypeSymbol* longType, ULongTypeSymbol* ulongType,
    FloatTypeSymbol* floatType, DoubleTypeSymbol* doubleType,
    CharTypeSymbol* charType, WCharTypeSymbol* wcharType, UCharTypeSymbol* ucharType)
{
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2sbyte", boolType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2byte", boolType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2short", boolType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2ushort", boolType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2int", boolType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2uint", boolType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2long", boolType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2ulong", boolType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"bool2float", boolType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"bool2double", boolType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2char", boolType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2wchar", boolType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"bool2uchar", boolType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"sbyte2bool", sbyteType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"sbyte2byte", sbyteType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeImplicitSignExtension, U"sbyte2short", 1, sbyteType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"sbyte2ushort", sbyteType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeImplicitSignExtension, U"sbyte2int", 2, sbyteType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"sbyte2uint", sbyteType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeImplicitSignExtension, U"sbyte2long", 3, sbyteType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"sbyte2ulong", sbyteType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"sbyte2float", 4, sbyteType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"sbyte2double", 5, sbyteType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"sbyte2char", sbyteType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"sbyte2wchar", sbyteType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"sbyte2uchar", sbyteType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"byte2bool", byteType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"byte2sbyte", byteType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"byte2short", 1, byteType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"byte2ushort", 2, byteType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"byte2int", 3, byteType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"byte2uint", 4, byteType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"byte2long", 5, byteType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"byte2long", 6, byteType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"byte2float", 7, byteType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"byte2double", 8, byteType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"byte2char", byteType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"byte2wchar", byteType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"byte2uchar", byteType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"short2bool", shortType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"short2sbyte", shortType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"short2byte", shortType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"short2ushort", shortType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeImplicitSignExtension, U"short2int", 1, shortType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"short2uint", shortType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeImplicitSignExtension, U"short2long", 2, shortType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"short2ulong", shortType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"short2float", 3, shortType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"short2double", 4, shortType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"short2char", shortType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"short2wchar", shortType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"short2uchar", shortType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ushort2bool", ushortType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ushort2sbyte", ushortType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ushort2byte", ushortType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"ushort2short", ushortType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"ushort2int", 1, ushortType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"ushort2uint", 2, ushortType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"ushort2long", 3, ushortType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"ushort2ulong", 4, ushortType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"ushort2float", 5, ushortType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"ushort2double", 6, ushortType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ushort2char", ushortType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"ushort2wchar", ushortType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"ushort2uchar", ushortType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"int2bool", intType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"int2sbyte", intType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"int2byte", intType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"int2short", intType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"int2ushort", intType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"int2uint", intType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeImplicitSignExtension, U"int2long", 1, intType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeSignExtension>(SymbolType::basicTypeExplicitSignExtension, U"int2ulong", intType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"int2float", 2, intType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"int2double", 3, intType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"int2char", intType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"int2wchar", intType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"int2uchar", intType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uint2bool", uintType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uint2sbyte", uintType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uint2byte", uintType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uint2short", uintType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uint2ushort", uintType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"uint2int", uintType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"uint2long", 1, uintType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"uint2ulong", 2, uintType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"uint2float", 3, uintType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"uint2double", 4, uintType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uint2char", uintType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uint2wchar", uintType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"uint2uchar", uintType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2bool", longType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"longsbyte", longType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2byte", longType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2short", longType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2ushort", longType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2int", longType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2uint", longType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"long2ulong", longType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"long2float", 1, longType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeSignedIntToFloating>(SymbolType::basicTypeImplicitSignedIntToFloating, U"long2double", 2, longType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2char", longType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2wchar", longType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"long2uchar", longType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2bool", ulongType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2sbyte", ulongType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2byte", ulongType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2short", ulongType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2ushort", ulongType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2int", ulongType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2uint", ulongType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"ulong2long", ulongType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"ulong2float", 1, ulongType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeImplicitUnsignedIntToFloating, U"ulong2double", 2, ulongType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2char", ulongType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2wchar", ulongType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"ulong2uchar", ulongType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2bool", floatType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"float2sbyte", floatType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2byte", floatType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"float2short", floatType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2ushort", floatType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"float2int", floatType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2uint", floatType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"float2long", floatType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2ulong", floatType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeFloatingExtension>(SymbolType::basicTypeFloatingExtension, U"float2double", 1, floatType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2char", floatType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2wchar", floatType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"float2uchar", floatType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2bool", doubleType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"double2sbyte", doubleType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2byte", doubleType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"double2short", doubleType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2ushort", doubleType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"double2int", doubleType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2uint", doubleType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToSignedInt>(SymbolType::basicTypeFloatingToSignedInt, U"double2long", doubleType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2ulong", doubleType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingTruncation>(SymbolType::basicTypeFloatingTruncation, U"double2float", doubleType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2char", doubleType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2wchar", doubleType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeFloatingToUnsignedInt>(SymbolType::basicTypeFloatingToUnsignedInt, U"double2uchar", doubleType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"char2bool", charType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"char2sbyte", charType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"char2byte", charType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"char2short", charType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"char2ushort", charType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"char2int", charType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"char2uint", charType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"char2long", charType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"char2ulong", charType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"char2float", charType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"char2double", charType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"char2wchar", 1, charType, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"char2uchar", 2, charType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"wchar2bool", wcharType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"wchar2sbyte", wcharType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"wchar2byte", wcharType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"wchar2short", wcharType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"wchar2ushort", wcharType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"wchar2int", wcharType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"wchar2uint", wcharType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"wchar2long", wcharType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"wchar2ulong", wcharType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"wchar2float", wcharType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"wchar2double", wcharType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"wchar2char", wcharType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeImplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeImplicitZeroExtension, U"wchar2uchar", 1, wcharType, ucharType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uchar2bool", ucharType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uchar2sbyte", ucharType, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uchar2byte", ucharType, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uchar2short", ucharType, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uchar2ushort", ucharType, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"uchar2int", ucharType, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeBitCast>(SymbolType::basicTypeBitCast, U"uchar2uint", ucharType, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"uchar2long", ucharType, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeZeroExtension>(SymbolType::basicTypeExplicitZeroExtension, U"uchar2ulong", ucharType, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"uchar2float", ucharType, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeUnsignedIntToFloating>(SymbolType::basicTypeExplicitUnsignedIntToFloating, U"uchar2double", ucharType, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uchar2char", ucharType, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeExplicitConversion<BasicTypeTruncation>(SymbolType::basicTypeTruncation, U"uchar2wchar", ucharType, wcharType));
}

void MakeBasicTypeOperations(SymbolTable& symbolTable,
    BoolTypeSymbol* boolType, SByteTypeSymbol* sbyteType, ByteTypeSymbol* byteType, ShortTypeSymbol* shortType, UShortTypeSymbol* ushortType, IntTypeSymbol* intType, UIntTypeSymbol* uintType,
    LongTypeSymbol* longType, ULongTypeSymbol* ulongType, FloatTypeSymbol* floatType, DoubleTypeSymbol* doubleType, CharTypeSymbol* charType, WCharTypeSymbol* wcharType, UCharTypeSymbol* ucharType, 
    VoidTypeSymbol* voidType)
{
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultInt1>(SymbolType::defaultInt1, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(boolType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(boolType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, boolType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeUnaryOperation<BasicTypeNot>(SymbolType::basicTypeNot, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultSInt8>(SymbolType::defaultSInt8, sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(sbyteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(sbyteType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(sbyteType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(sbyteType));
    MakeSignedIntegerTypeOperations(symbolTable, sbyteType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, sbyteType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeSignedIntegerLessThan>(SymbolType::basicTypeSignedIntegerLessThan, sbyteType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultUInt8>(SymbolType::defaultUInt8, byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(byteType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(byteType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(byteType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(byteType));
    MakeUnsignedIntegerTypeOperations(symbolTable, byteType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, byteType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan, byteType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultSInt16>(SymbolType::defaultSInt16, shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(shortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(shortType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(shortType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(shortType));
    MakeSignedIntegerTypeOperations(symbolTable, shortType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, shortType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeSignedIntegerLessThan>(SymbolType::basicTypeSignedIntegerLessThan, shortType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultUInt16>(SymbolType::defaultUInt16, ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(ushortType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(ushortType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(ushortType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(ushortType));
    MakeUnsignedIntegerTypeOperations(symbolTable, ushortType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, ushortType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan, ushortType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultSInt32>(SymbolType::defaultSInt32, intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(intType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(intType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(intType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(intType));
    MakeSignedIntegerTypeOperations(symbolTable, intType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, intType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeSignedIntegerLessThan>(SymbolType::basicTypeSignedIntegerLessThan, intType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultUInt32>(SymbolType::defaultUInt32, uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(uintType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(uintType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(uintType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(uintType));
    MakeUnsignedIntegerTypeOperations(symbolTable, uintType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, uintType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan, uintType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultSInt64>(SymbolType::defaultSInt64, longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(longType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(longType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(longType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(longType));
    MakeSignedIntegerTypeOperations(symbolTable, longType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, longType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeSignedIntegerLessThan>(SymbolType::basicTypeSignedIntegerLessThan, longType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultUInt64>(SymbolType::defaultUInt64, ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(ulongType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(ulongType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(ulongType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(ulongType));
    MakeUnsignedIntegerTypeOperations(symbolTable, ulongType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, ulongType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan, ulongType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultFloat>(SymbolType::defaultFloat, floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(floatType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(floatType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(floatType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(floatType));
    MakeFloatingPointTypeOperations(symbolTable, floatType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeFloatingEquality>(SymbolType::basicTypeFloatingEquality, floatType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeFloatingLessThan>(SymbolType::basicTypeFloatingLessThan, floatType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultDouble>(SymbolType::defaultDouble, doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(doubleType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(doubleType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(doubleType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(doubleType));
    MakeFloatingPointTypeOperations(symbolTable, doubleType);
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeFloatingEquality>(SymbolType::basicTypeFloatingEquality, doubleType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeFloatingLessThan>(SymbolType::basicTypeFloatingLessThan, doubleType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultChar>(SymbolType::defaultChar, charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(charType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(charType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(charType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, charType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan, charType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultWChar>(SymbolType::defaultWChar, wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(wcharType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(wcharType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(wcharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, wcharType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan, wcharType, boolType));

    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeDefaultCtor<DefaultUChar>(SymbolType::defaultUChar, ucharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyCtor(ucharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveCtor(ucharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeCopyAssignment(ucharType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeMoveAssignment(ucharType, voidType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeReturn(ucharType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeIntegerEquality>(SymbolType::basicTypeIntegerEquality, ucharType, boolType));
    symbolTable.AddFunctionSymbolToGlobalScope(new BasicTypeComparisonOperation<BasicTypeUnsignedIntegerLessThan>(SymbolType::basicTypeUnsignedIntegerLessThan, ucharType, boolType));

    MakeBasicTypeConversions(symbolTable, boolType, sbyteType, byteType, shortType, ushortType, intType, uintType, longType, ulongType, floatType, doubleType, charType, wcharType, ucharType);
}

} } // namespace cmajor::symbols
