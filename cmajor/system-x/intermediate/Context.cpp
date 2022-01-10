// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/IntermediateCodeLexer.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmsx::intermediate {

using namespace soulng::unicode;

Context::Context() : compileUnit(), lexer(nullptr)
{
    compileUnit.SetContext(this);
    types.SetContext(this);
    data.SetContext(this);
    code.SetContext(this);
    metadata.SetContext(this);
}

void Context::ReadFile(const std::string& filePath)
{
    content = ToUtf32(soulng::util::ReadFile(filePath));
}

void Context::ResetLexer(soulng::lexer::Lexer* lexer_)
{
    lexer.reset(lexer_);
}

void Context::SetFilePath(const std::string& filePath_)
{
    compileUnit.SetFilePath(filePath_);
}

const std::string& Context::FilePath() const
{
    return compileUnit.FilePath();
}

std::u32string Context::ErrorLines(const SourcePos& sourcePos)
{
    if (lexer)
    {
        return lexer->ErrorLines(sourcePos);
    }
    else
    {
        return std::u32string();
    }
}

void Context::SetCompileUnitInfo(const std::string& compileUnitId_, MetadataRef* metadataRef)
{
    compileUnit.SetInfo(compileUnitId_, metadataRef);
}

void Context::AddStructureType(const SourcePos& sourcePos, int32_t typeId, const std::vector<TypeRef>& fieldTypeRefs)
{
    types.AddStructureType(sourcePos, typeId, fieldTypeRefs);
}

void Context::AddArrayType(const SourcePos& sourcePos, int32_t typeId, int64_t size, const TypeRef& elementTypeRef)
{
    types.AddArrayType(sourcePos, typeId, size, elementTypeRef);
}

void Context::AddFunctionType(const SourcePos& sourcePos, int32_t typeId, const TypeRef& returnTypeRef, const std::vector<TypeRef>& paramTypeRefs)
{
    types.AddFunctionType(sourcePos, typeId, returnTypeRef, paramTypeRefs);
}

void Context::AddGlobalVariable(const SourcePos& sourcePos, Type* type, const std::string& variableName, ConstantValue* initializer, bool once)
{
    data.AddGlobalVariable(sourcePos, type, variableName, initializer, once, this);
}

void Context::ResolveTypes()
{
    types.Resolve(this);
}

void Context::ResolveType(TypeRef& typeRef)
{
    types.ResolveType(typeRef, this);
}

ConstantValue* Context::GetTrueValue() 
{
    return data.GetTrueValue(types);
}

ConstantValue* Context::GetFalseValue() 
{
    return data.GetFalseValue(types);
}

ConstantValue* Context::GetBooleanLiteral(const SourcePos& sourcePos, Type* type, bool value)
{
    if (type->IsBooleanType())
    {
        if (value)
        {
            return GetTrueValue();
        }
        else
        {
            return GetFalseValue();
        }
    }
    else
    {
        Error("error making literal value: Boolean type expected", sourcePos, this);
    }
    return nullptr;
}

ConstantValue* Context::GetSByteValue(int8_t value)
{
    return data.GetSByteValue(value, types);
}

ConstantValue* Context::GetByteValue(uint8_t value)
{
    return data.GetByteValue(value, types);
}

ConstantValue* Context::GetShortValue(int16_t value)
{
    return data.GetShortValue(value, types);
}

ConstantValue* Context::GetUShortValue(uint16_t value)
{
    return data.GetUShortValue(value, types);
}

ConstantValue* Context::GetIntValue(int32_t value)
{
    return data.GetIntValue(value, types);
}

ConstantValue* Context::GetUIntValue(uint32_t value)
{
    return data.GetUIntValue(value, types);
}

ConstantValue* Context::GetLongValue(int64_t value)
{
    return data.GetLongValue(value, types);
}

ConstantValue* Context::GetULongValue(uint64_t value)
{
    return data.GetULongValue(value, types);
}

ConstantValue* Context::GetFloatValue(float value)
{
    return data.GetFloatValue(value, types);
}

ConstantValue* Context::GetDoubleValue(double value)
{
    return data.GetDoubleValue(value, types);
}

ConstantValue* Context::GetNullValue(const SourcePos& sourcePos, Type* type)
{
    if (type->IsPointerType())
    {
        return data.GetNullValue(type);
    }
    else
    {
        Error("error making null literal value: pointer type expected", sourcePos, this);
    }
    return nullptr;
}

ConstantValue* Context::MakeArrayValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& elements)
{
    return data.MakeArrayValue(sourcePos, elements);
}

ConstantValue* Context::MakeStructureValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& fieldValues)
{
    return data.MakeStructureValue(sourcePos, fieldValues);
}

ConstantValue* Context::MakeStringValue(const SourcePos& sourcePos, const std::string& value)
{
    return data.MakeStringValue(sourcePos, value);
}

ConstantValue* Context::MakeStringArrayValue(const SourcePos& sourcePos, char prefix, const std::vector<ConstantValue*>& strings)
{
    return data.MakeStringArrayValue(sourcePos, prefix, strings);
}

ConstantValue* Context::MakeConversionValue(const SourcePos& sourcePos, Type* type, ConstantValue* from)
{
    return data.MakeConversionValue(sourcePos, type, from);
}

ConstantValue* Context::MakeClsIdValue(const SourcePos& sourcePos, Type* type, const std::string& clsIdStr)
{
    return data.MakeClsIdValue(sourcePos, type, clsIdStr);
}

ConstantValue* Context::MakeSymbolValue(const SourcePos& sourcePos, Type* type, const std::string& symbol)
{
    return data.MakeSymbolValue(sourcePos, type, symbol);
}

ConstantValue* Context::MakeNumericLiteral(const SourcePos& sourcePos, Type* type, const std::string& strValue)
{
    return data.MakeNumericLiteral(sourcePos, type, strValue, types, this);
}

ConstantValue* Context::MakeAddressLiteral(const SourcePos& sourcePos, Type* type, const std::string& id)
{
    return data.MakeAddressLiteral(sourcePos, type, id, this);
}

Function* Context::CurrentFunction() const
{
    return code.CurrentFunction();
}

void Context::ValidateData()
{
    // todo
}

void Context::SetCurrentFunction(Function* function)
{
    code.SetCurrentFunction(function);
}

Function* Context::AddFunctionDefinition(const SourcePos& sourcePos, Type* type, const std::string& functionId, bool once, MetadataRef* metadataRef)
{
    if (type->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(type);
        return code.AddFunctionDefinition(sourcePos, functionType, functionId, once, metadataRef, this);
    }
    else
    {
        Error("error adding function '" + functionId + "' definition: invalid type '" + type->Name() + "': function type expected", sourcePos, this);
    }
    return nullptr;
}

Function* Context::AddFunctionDeclaration(const SourcePos& sourcePos, Type* type, const std::string& functionId)
{
    if (type->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(type);
        return code.AddFunctionDeclaration(sourcePos, functionType, functionId);
    }
    else
    {
        Error("error adding function '" + functionId + "' declaration: invalid type '" + type->Name() + "': function type expected", sourcePos, this);
    }
    return nullptr;
}

MetadataStruct* Context::AddMetadataStruct(const SourcePos& sourcePos, int32_t id, Context* context)
{
    return metadata.AddMetadataStruct(sourcePos, id, context);
}

MetadataBool* Context::CreateMetadataBool(bool value)
{
    return metadata.CreateMetadataBool(value);
}

MetadataLong* Context::CreateMetadataLong(int64_t value)
{
    return metadata.CreateMetadataLong(value);
}

MetadataString* Context::CreateMetadataString(const std::string& value)
{
    return metadata.CreateMetadataString(value);
}

MetadataRef* Context::CreateMetadataRef(const SourcePos& sourcePos, int32_t nodeId)
{
    return metadata.CreateMetadataRef(sourcePos, nodeId);
}

void Context::ResolveMetadataReferences()
{
    metadata.ResolveMetadataReferences(this);
}

void Context::ResetRegisterPool()
{
    registerPool = RegisterPool();
}

} // cmsx::intermediate
