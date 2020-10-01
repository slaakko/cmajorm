// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngjson/json/JsonSchema.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/Path.hpp>
#include <fstream>
#include <stdexcept>

namespace sngjson { namespace json {

using namespace soulng::util;

Type::Type(const std::string& name_, const std::string& nativeName_) : name(name_), nativeName(nativeName_)
{
}

Type::~Type()
{
}

bool Type::WriteDeclaration(CodeFormatter& formatter)
{
    return false;
}

bool Type::WriteDefinition(CodeFormatter& formatter)
{
    return false;
}

IntType::IntType() : Type("int", "int32_t")
{
}

LongType::LongType() : Type("long", "int64_t")
{
}

DoubleType::DoubleType() : Type("double", "double")
{
}

FloatType::FloatType() : Type("float", "float")
{
}

BoolType::BoolType() : Type("bool", "bool")
{
}

StringType::StringType() : Type("string", "std::string")
{
}

Field::Field(Type* type_, const std::string& name_) : type(type_), name(name_)
{
}

StructType::StructType(const std::string& name, const std::string& api_) : Type(name, name), api(api_)
{
}

void StructType::AddField(Field* field)
{
    if (fieldMap.find(field->Name()) != fieldMap.cend())
    {
        throw std::runtime_error("already has field '" + field->Name() + "'");
    }
    fields.push_back(std::unique_ptr<Field>(field));
    fieldMap[field->Name()] = fields.back().get();
}

bool StructType::WriteDeclaration(CodeFormatter& formatter)
{
    std::string a;
    if (!api.empty())
    {
        a.append(" ").append(api);
    }
    formatter.WriteLine("struct" + a + " " + Name());
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine(Name() + "();");
    formatter.WriteLine(Name() + "(soulng::util::JsonValue* __json_value);");
    formatter.WriteLine("std::unique_ptr<soulng::util::JsonValue> ToJson() const;");
    for (const std::unique_ptr<Field>& field : fields)
    {
        formatter.WriteLine(field->GetType()->NativeName() + " " + field->Name() + ";");
    }
    formatter.DecIndent();
    formatter.WriteLine("};");
    return true;
}

bool StructType::WriteDefinition(CodeFormatter& formatter)
{
    formatter.Write(Name() + "::" + Name() + "()");
    if (!fields.empty())
    {
        formatter.Write(" : ");
    }
    bool first = true;
    for (const std::unique_ptr<Field>& field : fields)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        formatter.Write(field->Name() + "()");
    }
    formatter.WriteLine();
    formatter.WriteLine("{");
    formatter.WriteLine("}");
    formatter.WriteLine();
    formatter.WriteLine("std::unique_ptr<soulng::util::JsonValue> " + Name() + "::ToJson() const");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());");
    int index = 0;
    for (const std::unique_ptr<Field>& field : fields)
    {
        formatter.WriteLine("std::unique_ptr<soulng::util::JsonValue> fieldValue" + std::to_string(index) + " = sngjson::json::ToJson(" + field->Name() + ");");
        formatter.WriteLine("object->AddField(U\"" + field->Name() + "\", std::move(fieldValue" + std::to_string(index) + "));");
        ++index;
    }
    formatter.WriteLine("return object;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();
    formatter.WriteLine(Name() + "::" + Name() + "(soulng::util::JsonValue* __json_value)");
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (const std::unique_ptr<Field>& field : fields)
    {
        formatter.WriteLine("sngjson::json::FromJson(__json_value, \"" + field->Name() + "\", " + field->Name() + ");");
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    return true;
}

ArrayType::ArrayType(Type* elementType_) : Type("array of " + elementType_->Name(), "std::vector<" + elementType_->NativeName() + ">"), elementType(elementType_)
{
}

Context::Context() : intType(), doubleType(), boolType(), stringType()
{
    typeMap[intType.Name()] = &intType;
    typeMap[longType.Name()] = &longType;
    typeMap[doubleType.Name()] = &doubleType;
    typeMap[floatType.Name()] = &floatType;
    typeMap[boolType.Name()] = &boolType;
    typeMap[stringType.Name()] = &stringType;
}

Type* Context::GetTypeNoThrow(const std::string& name) const
{
    std::unordered_map<std::string, Type*>::const_iterator it = typeMap.find(name);
    if (it != typeMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

Type* Context::GetType(const std::string& name) const
{
    Type* prev = GetTypeNoThrow(name);
    if (prev)
    {
        return prev;
    }
    throw std::runtime_error("type '" + name + "' not found");
}

void Context::AddType(Type* type)
{
    Type* prev = GetTypeNoThrow(type->Name());
    if (prev != nullptr)
    {
        throw std::runtime_error("already has type '" + type->Name() + "'");
    }
    types.push_back(std::unique_ptr<Type>(type));
    typeMap[type->Name()] = type;
}

void Context::AddInclude(const std::string& include)
{
    includes.push_back(include);
}

void Context::Write(const std::string& hppFilePath, const std::string& cppFilePath)
{
    std::ofstream hppFile(hppFilePath);
    CodeFormatter hppFormatter(hppFile);
    std::string includeGuard = "HPP_" + soulng::util::GetSha1MessageDigest(hppFilePath);
    hppFormatter.WriteLine("#ifndef " + includeGuard);
    hppFormatter.WriteLine("#define " + includeGuard);
    hppFormatter.WriteLine("#include <soulng/util/Json.hpp>");
    for (const std::string& include : includes)
    {
        hppFormatter.WriteLine("#include " + include);
    }
    hppFormatter.WriteLine("#include <string>");
    hppFormatter.WriteLine("#include <vector>");
    hppFormatter.WriteLine("#include <memory>");
    hppFormatter.WriteLine();
    for (const std::unique_ptr<Type>& type : types)
    {
        bool written = type->WriteDeclaration(hppFormatter);
        if (written)
        {
            hppFormatter.WriteLine();
        }
    }
    hppFormatter.WriteLine("#endif // " + includeGuard);
    std::ofstream cppFile(cppFilePath);
    CodeFormatter cppFormatter(cppFile);
    cppFormatter.WriteLine("#include \"" + Path::GetFileName(hppFilePath) + "\"");
    cppFormatter.WriteLine("#include <sngjson/json/JsonImport.hpp>");
    cppFormatter.WriteLine("#include <sngjson/json/JsonExport.hpp>");
    cppFormatter.WriteLine("#include <utility>");
    cppFormatter.WriteLine();
    for (const std::unique_ptr<Type>& type : types)
    {
        bool written = type->WriteDefinition(cppFormatter);
        if (written)
        {
            cppFormatter.WriteLine();
        }
    }
}

} } // sngjson::json
