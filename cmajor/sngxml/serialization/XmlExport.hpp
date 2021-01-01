// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_EXPORT_INCLUDED
#define SNGXML_XML_EXPORT_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <memory>
#include <concepts>
#include <stdint.h>

namespace sngxml { namespace xmlser {

using namespace soulng::unicode;

using date = soulng::util::Date;
using datetime = soulng::util::DateTime;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;
using uuid = boost::uuids::uuid;

template<class T>
concept XmlExportableFundamentalType = std::is_fundamental_v<T>;

SNGXML_SERIALIZATION_API inline std::string ToString(int8_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(uint8_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(int16_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(uint16_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(int32_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(uint32_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(int64_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(uint64_t value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(float value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(double value)
{
    return std::to_string(value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(bool value)
{
    if (value) return "true"; else return "false";
}

SNGXML_SERIALIZATION_API inline std::string ToString(char value)
{
    return std::string(1, value);
}

SNGXML_SERIALIZATION_API inline std::string ToString(char16_t value)
{
    return ToUtf8(std::u16string(1, value));
}

SNGXML_SERIALIZATION_API inline std::string ToString(char32_t value)
{
    return ToUtf8(std::u32string(1, value));
}

SNGXML_SERIALIZATION_API inline std::string ToString(const date& ymd)
{
    return ymd.ToString();
}

SNGXML_SERIALIZATION_API inline std::string ToString(const datetime& dateTime)
{
    return dateTime.ToString();
}

SNGXML_SERIALIZATION_API inline std::string ToString(const time_point& tp)
{
    return ToString(std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count());
}

SNGXML_SERIALIZATION_API inline std::string ToString(const duration& dur)
{
    return ToString(std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count());
}

template<class T>
concept XmlExportableClassType = requires(T t, const std::string& u) { t.ToXml(u); };

template<class T>
concept XmlExportable =
requires { XmlExportableFundamentalType<T>; } || requires { XmlExportableClassType<T>;  };

template<XmlExportableFundamentalType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const T& value, const std::string& fieldName)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->SetAttribute(U"value", ToUtf32(ToString(value)));
    return element;
}

SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& value, const std::string& fieldName);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const std::u16string& value, const std::string& fieldName);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const std::u32string& value, const std::string& fieldName);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const uuid& value, const std::string& fieldName);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const date& value, const std::string& fieldName);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const datetime& value, const std::string& fieldName);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const time_point& value, const std::string& fieldName);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const duration& value, const std::string& fieldName);

SNGXML_SERIALIZATION_API std::string MakeClassNameStr(const std::string& typeName);

template<XmlExportableClassType T> 
std::unique_ptr<sngxml::dom::Element> ToXml(const T& object, const std::string& fieldName)
{
    std::unique_ptr<sngxml::dom::Element> element = object.ToXml(fieldName);
    element->SetAttribute(U"className", ToUtf32(MakeClassNameStr(typeid(T).name())));
    return element;
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::unique_ptr<T>& objectPtr, const std::string& fieldName)
{
    if (objectPtr)
    {
        std::unique_ptr<sngxml::dom::Element> element = objectPtr->ToXml(fieldName);
        T* p = objectPtr.get();
        element->SetAttribute(U"className", ToUtf32(MakeClassNameStr(typeid(*p).name())));
        return element;
    }
    else
    {
        std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
        element->SetAttribute(U"value", U"null");
        return element;
    }
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::shared_ptr<T>& objectPtr, const std::string& fieldName)
{
    if (objectPtr)
    {
        std::unique_ptr<sngxml::dom::Element> element = objectPtr->ToXml(fieldName);
        T* p = objectPtr.get();
        element->SetAttribute(U"className", ToUtf32(typeid(*p).name()));
        return element;
    }
    else
    {
        std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
        element->SetAttribute(U"value", U"null");
        return element;
    }
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<T>& v, const std::string& fieldName)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const T& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item").release()));
    }
    return element;
}

template<XmlExportableFundamentalType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<T>& v, const std::string& fieldName)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const T& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item").release()));
    }
    return element;
}


template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<std::unique_ptr<T>>& v, const std::string& fieldName)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const std::unique_ptr<T>& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item").release()));
    }
    return element;
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<std::shared_ptr<T>>& v, const std::string& fieldName)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const std::shared_ptr<T>& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item").release()));
    }
    return element;
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_EXPORT_INCLUDED
