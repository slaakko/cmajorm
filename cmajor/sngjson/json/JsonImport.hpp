// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGJSON_JSON_IMPORT_INCLUDED
#define SNGJSON_JSON_IMPORT_INCLUDED
#include <sngjson/json/JsonApi.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Unicode.hpp>
#include <string>
#include <vector>

namespace sngjson { namespace json {

SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, int32_t& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, int64_t& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, double& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, float& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, bool& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, std::string& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, std::u16string& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* value, std::u32string& field);

SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, int32_t& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, int64_t& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, double& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, float& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, bool& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::string& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::u16string& field);
SNGJSON_JSON_API void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::u32string& field);

template<typename T>
concept JsonConstructible = requires(soulng::util::JsonValue * value) { T(value); };

template<typename T>
requires JsonConstructible<T>
    void FromJson(soulng::util::JsonValue* value, T& field);

template<typename T>
requires JsonConstructible<T>
    void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, T& field);

template<typename T>
void FromJson(soulng::util::JsonValue* value, std::vector<T>& field);

template<typename T>
void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::vector<T>& field);

template<typename T>
requires JsonConstructible<T>
    void FromJson(soulng::util::JsonValue* value, T& field)
{
    if (value)
    {
        field = T(value);
    }
    else
    {
        field = T();
    }
}

template<typename T>
requires JsonConstructible<T>
    void FromJson(soulng::util::JsonValue* value, std::vector<T>& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::array)
        {
            soulng::util::JsonArray* a = static_cast<soulng::util::JsonArray*>(value);
            int n = a->Count();
            for (int i = 0; i < n; ++i)
            {
                soulng::util::JsonValue* itemValue = (*a)[i];
                T item(itemValue);
                field.push_back(item);
            }
        }
    }
    else
    {
        field = std::vector<T>();
    }
}

template<typename T>
void FromJson(soulng::util::JsonValue* value, std::vector<T>& field)
{
    if (value)
    {
        if (value->Type() == soulng::util::JsonValueType::array)
        {
            soulng::util::JsonArray* a = static_cast<soulng::util::JsonArray*>(value);
            int n = a->Count();
            for (int i = 0; i < n; ++i)
            {
                soulng::util::JsonValue* itemValue = (*a)[i];
                T item;
                FromJson(itemValue, item);
                field.push_back(item);
            }
        }
    }
    else
    {
        field = std::vector<T>();
    }
}

template<typename T>
requires JsonConstructible<T>
    void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, T& field)
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
            field = T();
        }
    }
    else
    {
        field = T();
    }
}

template<typename T>
void FromJson(soulng::util::JsonValue* container, const std::string& fieldName, std::vector<T>& field)
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
            field = std::vector<T>();
        }
    }
    else
    {
        field = std::vector<T>();
    }
}

} } // sngjson::json

#endif // SNGJSON_JSON_IMPORT_INCLUDED
