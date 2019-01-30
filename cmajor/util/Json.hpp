// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_JSON_INCLUDED
#define CMAJOR_UTIL_JSON_INCLUDED
#include <cmajor/util/UtilApi.hpp>
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace cmajor { namespace util {

enum class JsonValueType
{
    object, array, string, number, boolean, null
};

class UTIL_API JsonValue
{
public:
    JsonValue(JsonValueType type_);
    JsonValue(const JsonValue&) = delete;
    JsonValue& operator=(const JsonValue&) = delete;
    virtual ~JsonValue();
    JsonValueType Type() const { return type; }
    virtual std::string ToString() const = 0;
private:
    JsonValueType type;
};

class UTIL_API JsonString : public JsonValue
{
public:
    JsonString();
    JsonString(const std::u32string& value_);
    void Append(char32_t c);
    const std::u32string& Value() const { return value; }
    std::u16string JsonCharStr(char32_t c) const;
    std::string ToString() const override;
private:
    std::u32string value;
};

class UTIL_API JsonNumber : public JsonValue
{
public:
    JsonNumber();
    JsonNumber(double value_);
    double Value() const { return value; }
    std::string ToString() const override;
private:
    double value;
};

class UTIL_API JsonBool : public JsonValue
{
public:
    JsonBool();
    JsonBool(bool value_);
    bool Value() const { return value; }
    std::string ToString() const override;
private:
    bool value;
};

class UTIL_API JsonObject : public JsonValue
{
public:
    JsonObject();
    void AddField(const std::u32string& fieldName, std::unique_ptr<JsonValue>&& fieldValue);
    JsonValue* GetField(const std::u32string& fieldName);
    std::string ToString() const override;
private:
    std::vector<std::unique_ptr<JsonValue>> fieldValues;
    std::map<std::u32string, JsonValue*> fieldMap;
};

class UTIL_API JsonArray : public JsonValue
{
public:
    JsonArray();
    void AddItem(std::unique_ptr<JsonValue>&& item);
    int Count() const { return items.size(); }
    JsonValue* operator[](int index) const;
    std::string ToString() const override;
private:
    std::vector<std::unique_ptr<JsonValue>> items;
};

class UTIL_API JsonNull : public JsonValue
{
public:
    JsonNull();
    std::string ToString() const override;
};

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_JSON_INCLUDED
