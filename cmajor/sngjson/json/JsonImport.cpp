// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngjson/json/JsonImport.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngjson { namespace json {

void FromJson(soulng::util::JsonValue* value, int32_t& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::number)
        {
            soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
            field = static_cast<int32_t>(number->Value());
        }
        else
        {
            field = int32_t();
        }
    }
    else
    {
        field = int32_t();
    }
}

void FromJson(soulng::util::JsonValue* value, int64_t& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::number)
        {
            soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
            field = static_cast<int64_t>(number->Value());
        }
        else
        {
            field = int64_t();
        }
    }
    else
    {
        field = int64_t();
    }
}

void FromJson(soulng::util::JsonValue* value, double& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::number)
        {
            soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
            field = number->Value();
        }
        else
        {
            field = double();
        }
    }
    else
    {
        field = double();
    }
}

void FromJson(soulng::util::JsonValue* value, float& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::number)
        {
            soulng::util::JsonNumber* number = static_cast<soulng::util::JsonNumber*>(value);
            field = static_cast<float>(number->Value());
        }
        else
        {
            field = float();
        }
    }
    else
    {
        field = float();
    }
}

void FromJson(soulng::util::JsonValue* value, bool& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::boolean)
        {
            soulng::util::JsonBool* b = static_cast<soulng::util::JsonBool*>(value);
            field = b->Value();
        }
        else
        {
            field = bool();
        }
    }
    else
    {
        field = bool();
    }
}

void FromJson(soulng::util::JsonValue* value, std::string& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::string)
        {
            soulng::util::JsonString* s = static_cast<soulng::util::JsonString*>(value);
            field = soulng::unicode::ToUtf8(s->Value());
        }
        else
        {
            field = std::string();
        }
    }
    else
    {
        field = std::string();
    }
}

void FromJson(soulng::util::JsonValue* value, std::u16string& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::string)
        {
            soulng::util::JsonString* s = static_cast<soulng::util::JsonString*>(value);
            field = soulng::unicode::ToUtf16(s->Value());
        }
        else
        {
            field = std::u16string();
        }
    }
    else
    {
        field = std::u16string();
    }
}

void FromJson(soulng::util::JsonValue* value, std::u32string& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::string)
        {
            soulng::util::JsonString* s = static_cast<soulng::util::JsonString*>(value);
            field = s->Value();
        }
        else
        {
            field = std::u32string();
        }
    }
    else
    {
        field = std::u32string();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, int32_t& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = int32_t();
        }
    }
    else
    {
        field = int32_t();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, int64_t& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = int64_t();
        }
    }
    else
    {
        field = int64_t();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, double& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = double();
        }
    }
    else
    {
        field = double();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, float& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = float();
        }
    }
    else
    {
        field = float();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, bool& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = bool();
        }
    }
    else
    {
        field = bool();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::string& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = std::string();
        }
    }
    else
    {
        field = std::string();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::u16string& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = std::u16string();
        }
    }
    else
    {
        field = std::u16string();
    }
}

void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::u32string& field)
{
    if (container)
    {
        soulng::util::JsonObject* object = static_cast<soulng::util::JsonObject*>(container);
        soulng::util::JsonValue* value = object->GetField(soulng::unicode::ToUtf32(fieldName));
        if (value)
        {
            FromJson(value, field);
        }
        else
        {
            field = std::u32string();
        }
    }
    else
    {
        field = std::u32string();
    }
}

} } // sngjson::json
