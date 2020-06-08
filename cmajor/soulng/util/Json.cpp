// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Json.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace soulng { namespace util {

using namespace soulng::unicode;

JsonValue::JsonValue(JsonValueType type_) : type(type_)
{
}

JsonValue::~JsonValue()
{
}

void JsonValue::Write(CodeFormatter& formatter)
{
    formatter.Write(ToString());
}

JsonString::JsonString() : JsonValue(JsonValueType::string), value()
{
}

JsonString::JsonString(const std::u32string& value_) : JsonValue(JsonValueType::string), value(value_)
{
}

void JsonString::Append(char32_t c)
{
    value.append(1, c);
}

std::u16string JsonString::JsonCharStr(char32_t c) const
{
    std::u16string result;
    std::u32string s(1, c);
    std::u16string t = ToUtf16(s);
    for (char16_t x : t)
    {
        switch (x)
        {
            case '"': result.append(u"\\\""); break;
            case '\\': result.append(u"\\\\"); break;
            case '/': result.append(u"\\/"); break;
            case '\b': result.append(u"\\b"); break;
            case '\f': result.append(u"\\f"); break;
            case '\n': result.append(u"\\n"); break;
            case '\r': result.append(u"\\r"); break;
            case '\t': result.append(u"\\t"); break;
            default:
            {
                if (x >= 32 && x <= 126)
                {
                    result.append(1, x);
                }
                else
                {
                    result.append(u"\\u").append(ToUtf16(ToHexString(static_cast<uint16_t>(x))));
                }
            }
        }
    }
    return result;
}

std::string JsonString::ToString() const
{
    std::u16string s;
    for (char32_t c : value)
    {
        s.append(JsonCharStr(c));
    }
    return "\"" + ToUtf8(s) + "\"";
}

JsonNumber::JsonNumber() : JsonValue(JsonValueType::number), value(0.0)
{
}

JsonNumber::JsonNumber(double value_) : JsonValue(JsonValueType::number), value(value_)
{
}

std::string JsonNumber::ToString() const
{
    return std::to_string(value);
}

JsonBool::JsonBool() : JsonValue(JsonValueType::boolean), value(false)
{
}

JsonBool::JsonBool(bool value_) : JsonValue(JsonValueType::boolean), value(value_)
{
}

std::string JsonBool::ToString() const
{
    return value ? "true" : "false";
}

JsonObject::JsonObject() : JsonValue(JsonValueType::object), fieldValues(), fieldMap()
{
}

void JsonObject::AddField(const std::u32string& fieldName, std::unique_ptr<JsonValue>&& fieldValue)
{
    fieldMap[fieldName] = fieldValue.get();
    fieldValues.push_back(std::move(fieldValue));
}

JsonValue* JsonObject::GetField(const std::u32string& fieldName)
{
    auto it = fieldMap.find(fieldName);
    if (it != fieldMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

std::string JsonObject::GetStringField(const std::u32string& fieldName)
{
    JsonValue* value = GetField(fieldName);
    if (value)
    {
        if (value->Type() == JsonValueType::string)
        {
            return value->ToString();
        }
        else
        {
            throw std::runtime_error("error getting field " + ToUtf8(fieldName) + ": string field expected");
        }
    }
    else
    {
        return std::string();
    }
}

std::string JsonObject::ToString() const
{
    std::string str = "{";
    bool first = true;
    for (const auto& p : fieldMap)
    {
        JsonString s(p.first);
        JsonValue* v(p.second);
        if (first)
        {
            first = false;
        }
        else
        {
            str.append(", ");
        }
        str.append(s.ToString()).append(":").append(v->ToString());
    }
    str.append("}");
    return str;
}

void JsonObject::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("{");
    formatter.IncIndent();
    JsonValueType lastItemType = JsonValueType::object;
    bool first = true;
    for (const auto& p : fieldMap)
    {
        JsonString s(p.first);
        JsonValue* v(p.second);
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.WriteLine(", ");
        }
        s.Write(formatter);
        formatter.Write(" : ");
        if (v->Type() == JsonValueType::array || v->Type() == JsonValueType::object)
        {
            formatter.WriteLine();
        }
        if (s.Value() == U"content")
        {
            formatter.BeginContent();
        }
        v->Write(formatter);
        if (s.Value() == U"content")
        {
            formatter.EndContent();
        }
        lastItemType = v->Type();
    }
    formatter.DecIndent();
    if (lastItemType != JsonValueType::array && lastItemType != JsonValueType::object)
    {
        formatter.WriteLine();
    }
    formatter.WriteLine("}");
}

JsonArray::JsonArray() : JsonValue(JsonValueType::array)
{
}

void JsonArray::AddItem(std::unique_ptr<JsonValue>&& item)
{
    items.push_back(std::move(item));
}

JsonValue* JsonArray::operator[](int index) const
{
    return items[index].get();
}

std::string JsonArray::ToString() const
{
    std::string str = "[";
    bool first = true;
    for (const std::unique_ptr<JsonValue>& item : items)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            str.append(", ");
        }
        str.append(item->ToString());
    }
    str.append("]");
    return str;
}

void JsonArray::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("[");
    formatter.IncIndent();
    JsonValueType lastItemType = JsonValueType::array;
    bool first = true;
    for (const std::unique_ptr<JsonValue>& item : items)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.WriteLine(", ");
        }
        item->Write(formatter);
        lastItemType = item->Type();
    }
    formatter.DecIndent();
    if (lastItemType != JsonValueType::array && lastItemType != JsonValueType::object)
    {
        formatter.WriteLine();
    }
    formatter.WriteLine("]");
}

JsonNull::JsonNull() : JsonValue(JsonValueType::null)
{
}

std::string JsonNull::ToString() const
{
    return "null";
}

} } // namespace soulng::util
