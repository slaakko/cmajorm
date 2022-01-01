// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_CONTEXT_INCLUDED
#define CMSX_INTERMEDIATE_CONTEXT_INCLUDED
#include <system-x/intermediate/CompileUnit.hpp>
#include <system-x/intermediate/Types.hpp>
#include <system-x/intermediate/Data.hpp>
#include <system-x/intermediate/Code.hpp>
#include <system-x/intermediate/Metadata.hpp>
#include <soulng/lexer/Lexer.hpp>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API Context
{
public:
    Context();
    void ReadFile(const std::string& filePath);
    const char32_t* Start() const { return content.c_str(); }
    const char32_t* End() const { return content.c_str() + content.length(); }
    CompileUnit& GetCompileUnit() { return compileUnit; }
    Types& GetTypes() { return types; }
    Data& GetData() { return data; }
    Code& GetCode() { return code; }
    Metadata& GetMetadata() { return metadata; }
    void ResetLexer(soulng::lexer::Lexer* lexer_);
    void SetFilePath(const std::string& filePath_);
    const std::string& FilePath() const;
    std::u32string ErrorLines(const SourcePos& sourcePos);
    void SetCompileUnitInfo(const std::string& compileUnitId_, MetadataRef* metadataRef);
    void AddStructureType(const SourcePos& sourcePos, int32_t typeId, const std::vector<TypeRef>& fieldTypeRefs);
    void AddArrayType(const SourcePos& sourcePos, int32_t typeId, int64_t size, const TypeRef& elementTypeRef);
    void AddFunctionType(const SourcePos& sourcePos, int32_t typeId, const TypeRef& returnTypeRef, const std::vector<TypeRef>& paramTypeRefs);
    void AddGlobalVariable(const SourcePos& sourcePos, Type* type, const std::string& variableName, ConstantValue* initializer, bool once);
    void ResolveTypes();
    void ValidateData();
    void ResolveType(TypeRef& typeRef);
    ConstantValue* GetTrueValue();
    ConstantValue* GetFalseValue();
    ConstantValue* GetBooleanLiteral(const SourcePos& sourcePos, Type* type, bool value);
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
    ConstantValue* GetNullValue(const SourcePos& sourcePos, Type* type);
    ConstantValue* MakeArrayValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& elements);
    ConstantValue* MakeStructureValue(const SourcePos& sourcePos, const std::vector<ConstantValue*>& fieldValues);
    ConstantValue* MakeStringValue(const SourcePos& sourcePos, const std::string& value);
    ConstantValue* MakeStringArrayValue(const SourcePos& sourcePos, char prefix, const std::vector<ConstantValue*>& strings);
    ConstantValue* MakeConversionValue(const SourcePos& sourcePos, Type* type, ConstantValue* from);
    ConstantValue* MakeClsIdValue(const SourcePos& sourcePos, Type* type, const std::string& clsIdStr);
    ConstantValue* MakeSymbolValue(const SourcePos& sourcePos, Type* type, const std::string& symbol);
    ConstantValue* MakeNumericLiteral(const SourcePos& sourcePos, Type* type, const std::string& strValue);
    ConstantValue* MakeAddressLiteral(const SourcePos& sourcePos, Type* type, const std::string& id);
    Function* CurrentFunction() const;
    void SetCurrentFunction(Function* function);
    Function* AddFunctionDefinition(const SourcePos& sourcePos, Type* type, const std::string& functionId, bool once, MetadataRef* metadataRef);
    Function* AddFunctionDeclaration(const SourcePos& sourcePos, Type* type, const std::string& functionId);
    MetadataStruct* AddMetadataStruct(const SourcePos& sourcePos, int32_t id, Context* context);
    MetadataBool* CreateMetadataBool(bool value);
    MetadataLong* CreateMetadataLong(int64_t value);
    MetadataString* CreateMetadataString(const std::string& value);
    MetadataRef* CreateMetadataRef(const SourcePos& sourcePos, int32_t nodeId);
    void ResolveMetadataReferences();
private:
    std::u32string content;
    std::unique_ptr<soulng::lexer::Lexer> lexer;
    CompileUnit compileUnit;
    Types types;
    Data data;
    Code code;
    Metadata metadata;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_CONTEXT_INCLUDED
