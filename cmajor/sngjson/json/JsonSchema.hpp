// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGJSON_JSON_SCHEMA_INCLUDED
#define SNGJSON_JSON_SCHEMA_INCLUDED
#include <sngjson/json/JsonApi.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace sngjson { namespace json {

using soulng::util::CodeFormatter;

class SNGJSON_JSON_API Type
{
public:
    Type(const std::string& name_, const std::string& nativeName_);
    virtual ~Type();
    const std::string& Name() const { return name; }
    const std::string& NativeName() const { return nativeName; }
    virtual bool WriteDeclaration(CodeFormatter& formatter);
    virtual bool WriteDefinition(CodeFormatter& formatter);
private:
    std::string name;
    std::string nativeName;
};

class SNGJSON_JSON_API IntType : public Type
{
public:
    IntType();
};

class SNGJSON_JSON_API LongType : public Type
{
public:
    LongType();
};

class SNGJSON_JSON_API DoubleType : public Type
{
public:
    DoubleType();
};

class SNGJSON_JSON_API FloatType : public Type
{
public:
    FloatType();
};

class SNGJSON_JSON_API BoolType : public Type
{
public:
    BoolType();
};

class SNGJSON_JSON_API StringType : public Type
{
public:
    StringType();
};

class SNGJSON_JSON_API Field
{
public:
    Field(Type* type_, const std::string& name_);
    Field(const Field&) = delete;
    Field& operator=(const Field&) = delete;
    Type* GetType() const { return type; }
    const std::string& Name() const { return name; }
private:
    Type* type;
    std::string name;
};

class SNGJSON_JSON_API StructType : public Type
{
public:
    StructType(const std::string& name, const std::string& api_);
    StructType(const StructType&) = delete;
    StructType& operator=(const StructType&) = delete;
    void AddField(Field* field);
    bool WriteDeclaration(CodeFormatter& formatter) override;
    bool WriteDefinition(CodeFormatter& formatter) override;
private:
    std::string api;
    std::vector<std::unique_ptr<Field>> fields;
    std::unordered_map<std::string, Field*> fieldMap;
};

class SNGJSON_JSON_API ArrayType : public Type
{
public:
    ArrayType(Type* elementType_);
    ArrayType(const ArrayType&) = delete;
    ArrayType& operator=(const ArrayType&) = delete;
private:
    Type* elementType;
};

class SNGJSON_JSON_API Context
{
public:
    Context();
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Type* GetTypeNoThrow(const std::string& name) const;
    Type* GetType(const std::string& name) const;
    void AddType(Type* type);
    void AddInclude(const std::string& include);
    void Write(const std::string& hppFilePath, const std::string& cppFilePath);
private:
    std::vector<std::string> includes;
    std::unordered_map<std::string, Type*> typeMap;
    std::vector<std::unique_ptr<Type>> types;
    IntType intType;
    LongType longType;
    DoubleType doubleType;
    FloatType floatType;
    BoolType boolType;
    StringType stringType;
};

} } // sngjson::json

#endif // SNGJSON_JSON_SCHEMA_INCLUDED
