// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGJSON_JSON_EXPORT_INCLUDED
#define SNGJSON_JSON_EXPORT_INCLUDED
#include <soulng/util/Json.hpp>
#include <sngjson/json/JsonApi.hpp>
#include <memory>
#include <string>
#include <vector>

namespace sngjson { namespace json {

SNGJSON_JSON_API std::unique_ptr<soulng::util::JsonValue> ToJson(double value);
SNGJSON_JSON_API std::unique_ptr<soulng::util::JsonValue> ToJson(bool value);
SNGJSON_JSON_API std::unique_ptr<soulng::util::JsonValue> ToJson(const std::string& value);
SNGJSON_JSON_API std::unique_ptr<soulng::util::JsonValue> ToJson(const std::u16string& value);
SNGJSON_JSON_API std::unique_ptr<soulng::util::JsonValue> ToJson(const std::u32string& value);

template<typename T>
concept JsonExportable = requires(T t) { t.ToJson(); };

template<typename T>
    requires JsonExportable<T>
std::unique_ptr<soulng::util::JsonValue> ToJson(const T& value)
{
    return value.ToJson();
}

template<class T>
std::unique_ptr<soulng::util::JsonValue> ToJson(const std::vector<T>& value)
{
    std::unique_ptr<soulng::util::JsonArray> val(new soulng::util::JsonArray());
    int n = value.size();
    for (int i = 0; i < n; ++i)
    {
        val->AddItem(ToJson(value[i]));
    }
    return val;
}

} } // sngjson::json

#endif // SNGJSON_JSON_EXPORT_INCLUDED
