#include <sngjson/json/JsonExport.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngjson { namespace json {

std::unique_ptr<soulng::util::JsonValue> ToJson(double value)
{
    return std::unique_ptr<soulng::util::JsonValue>(new soulng::util::JsonNumber(value));
}

std::unique_ptr<soulng::util::JsonValue> ToJson(bool value)
{
    return std::unique_ptr<soulng::util::JsonValue>(new soulng::util::JsonBool(value));
}

std::unique_ptr<soulng::util::JsonValue> ToJson(const std::string& value)
{
    return ToJson(soulng::unicode::ToUtf32(value));
}

std::unique_ptr<soulng::util::JsonValue> ToJson(const std::u16string& value)
{
    return ToJson(soulng::unicode::ToUtf32(value));
}

std::unique_ptr<soulng::util::JsonValue> ToJson(const std::u32string& value)
{
    return std::unique_ptr<soulng::util::JsonValue>(new soulng::util::JsonString(value));
}

} } // sngjson::json

