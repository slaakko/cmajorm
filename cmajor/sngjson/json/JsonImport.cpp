// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngjson/json/JsonImport.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngjson { namespace json {

void FromJson(soulng::util::JsonValue* value, int32_t& field)
{
    if (value->Type() == soulng::util::JsonValueType::number)
    {
        soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
        field = static_cast<int32_t>(number->Value());
    }
}

void FromJson(soulng::util::JsonValue* value, int64_t& field)
{
    if (value->Type() == soulng::util::JsonValueType::number)
    {
        soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
        field = static_cast<int64_t>(number->Value());
    }
}

void FromJson(soulng::util::JsonValue* value, double& field)
{
    if (value->Type() == soulng::util::JsonValueType::number)
    {
        soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
        field = number->Value();
    }
}

void FromJson(soulng::util::JsonValue* value, float& field)
{
    if (value->Type() == soulng::util::JsonValueType::number)
    {
        soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
        field = static_cast<float>(number->Value());
    }
}

void FromJson(soulng::util::JsonValue* value, bool& field)
{
    if (value->Type() == soulng::util::JsonValueType::boolean)
    {
        soulng::util::JsonBool* b = static_cast<soulng::util::JsonBool*>(value);
        field = b->Value();
    }
}

void FromJson(soulng::util::JsonValue* value, std::string& field)
{
    if (value->Type() == soulng::util::JsonValueType::string)
    {
        soulng::util::JsonString* s = static_cast<soulng::util::JsonString*>(value);
        field = soulng::unicode::ToUtf8(s->Value());
    }
}

void FromJson(soulng::util::JsonValue* value, std::u16string& field)
{
    if (value->Type() == soulng::util::JsonValueType::string)
    {
        soulng::util::JsonString* s = static_cast<soulng::util::JsonString*>(value);
        field = soulng::unicode::ToUtf16(s->Value());
    }
}

void FromJson(soulng::util::JsonValue* value, std::u32string& field)
{
    if (value->Type() == soulng::util::JsonValueType::string)
    {
        soulng::util::JsonString* s = static_cast<soulng::util::JsonString*>(value);
        field = s->Value();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, int32_t& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, int64_t& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, double& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, float& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, bool& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::string& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::u16string& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::u32string& field)
{
    soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
    soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
    FromJson(value, field);
}

} } // sngjson::json
