// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_EXPORT_INCLUDED
#define SNGXML_XML_EXPORT_INCLUDED
#include <sngxml/serialization/XmlPtr.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <memory>
#include <stdint.h>
#ifdef _WIN32
#include <concepts>
#endif

namespace sngxml { namespace xmlser {

using namespace soulng::unicode;

using date = soulng::util::Date;
using datetime = soulng::util::DateTime;
using timestamp = soulng::util::Timestamp;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;
using uuid = boost::uuids::uuid;
template<class T> using xml_ptr = sngxml::xmlser::XmlPtr<T>;
template<class T> using unique_xml_ptr = sngxml::xmlser::UniqueXmlPtr<T>;

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

SNGXML_SERIALIZATION_API inline std::string ToString(const timestamp& ts)
{
    return ts.ToString();
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
concept XmlExportableClassType = std::is_class_v<T> && requires(T t, const std::string& u, XmlSerializationContext& c) { t.ToXml(u, c); };

template<class T>
concept XmlExportableEnumType = std::is_enum_v<T>;

template<class T>
concept XmlExportableScalarType = 
std::is_fundamental_v<T> ||
std::is_same_v<T, std::string> ||
std::is_same_v<T, std::u16string> ||
std::is_same_v<T, std::u32string> ||
std::is_same_v<T, uuid> ||
std::is_same_v<T, date> ||
std::is_same_v<T, datetime> ||
std::is_same_v<T, timestamp> ||
std::is_same_v<T, time_point> ||
std::is_same_v<T, duration>;

template<class T>
concept XmlExportable =
requires { XmlExportableScalarType<T>; } || requires { XmlExportableClassType<T>;  };

template<XmlExportableScalarType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const T& value, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->SetAttribute(U"value", ToUtf32(ToString(value)));
    return element;
}

SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const std::u16string& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const std::u32string& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const uuid& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const date& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const datetime& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const timestamp& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const time_point& value, const std::string& fieldName, XmlSerializationContext& ctx);
SNGXML_SERIALIZATION_API std::unique_ptr<sngxml::dom::Element> ToXml(const duration& value, const std::string& fieldName, XmlSerializationContext& ctx);

SNGXML_SERIALIZATION_API std::string MakeClassNameStr(const std::string& typeName);

template<XmlExportableClassType T> 
std::unique_ptr<sngxml::dom::Element> ToXml(const T& object, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element = object.ToXml(fieldName, ctx);
    if (!ctx.GetFlag(XmlSerializationFlags::suppressMetadata))
    {
        if (const XmlSerializable* intf = dynamic_cast<const XmlSerializable*>(&object))
        {
            element->SetAttribute(U"classId", ToUtf32(std::to_string(intf->ClassId())));
            element->SetAttribute(U"className", ToUtf32(intf->ClassName()));
        }
    }
    return element;
}

template<XmlExportableEnumType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const T& value, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->SetAttribute(U"value", ToUtf32(std::to_string(static_cast<int64_t>(value))));
    return element;
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::unique_ptr<T>& objectPtr, const std::string& fieldName, XmlSerializationContext& ctx)
{
    if (objectPtr)
    {
        std::unique_ptr<sngxml::dom::Element> element = objectPtr->ToXml(fieldName, ctx);
        if (!ctx.GetFlag(XmlSerializationFlags::suppressMetadata))
        {
            T* p = objectPtr.get();
            if (XmlSerializable* intf = dynamic_cast<XmlSerializable*>(p))
            {
                element->SetAttribute(U"classId", ToUtf32(std::to_string(intf->ClassId())));
                element->SetAttribute(U"className", ToUtf32(intf->ClassName()));
            }
        }
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
std::unique_ptr<sngxml::dom::Element> ToXml(const std::shared_ptr<T>& objectPtr, const std::string& fieldName, XmlSerializationContext& ctx)
{
    if (objectPtr)
    {
        std::unique_ptr<sngxml::dom::Element> element = objectPtr->ToXml(fieldName, ctx);
        if (!ctx.GetFlag(XmlSerializationFlags::suppressMetadata))
        {
            T* p = objectPtr.get();
            if (XmlSerializable* intf = dynamic_cast<XmlSerializable*>(p))
            {
                element->SetAttribute(U"classId", ToUtf32(std::to_string(intf->ClassId())));
                element->SetAttribute(U"className", ToUtf32(intf->ClassName()));
            }
        }
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
std::unique_ptr<sngxml::dom::Element> ToXml(const sngxml::xmlser::XmlPtr<T>& xmlPtr, const std::string& fieldName, XmlSerializationContext& ctx)
{
    if (xmlPtr.Get())
    {
        std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
        element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(xmlPtr.TargetObjectId())));
        return element;
    }
    else
    {
        std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
        element->SetAttribute(U"value", U"null");
        element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(xmlPtr.TargetObjectId())));
        return element;
    }
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const sngxml::xmlser::UniqueXmlPtr<T>& xmlPtr, const std::string& fieldName, XmlSerializationContext& ctx)
{
    if (xmlPtr.Get())
    {
        std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
        element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(xmlPtr.TargetObjectId())));
        return element;
    }
    else
    {
        std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
        element->SetAttribute(U"value", U"null");
        element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(xmlPtr.TargetObjectId())));
        return element;
    }
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<T>& v, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const T& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item", ctx).release()));
    }
    return element;
}

template<XmlExportableScalarType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<T>& v, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const T& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item", ctx).release()));
    }
    return element;
}


template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<std::unique_ptr<T>>& v, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const std::unique_ptr<T>& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item", ctx).release()));
    }
    return element;
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<std::shared_ptr<T>>& v, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const std::shared_ptr<T>& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item", ctx).release()));
    }
    return element;
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<sngxml::xmlser::XmlPtr<T>>& v, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const sngxml::xmlser::XmlPtr<T>& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item", ctx).release()));
    }
    return element;
}

template<XmlExportableClassType T>
std::unique_ptr<sngxml::dom::Element> ToXml(const std::vector<sngxml::xmlser::UniqueXmlPtr<T>>& v, const std::string& fieldName, XmlSerializationContext& ctx)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    for (const sngxml::xmlser::UniqueXmlPtr<T>& item : v)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml(item, "item", ctx).release()));
    }
    return element;
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_EXPORT_INCLUDED
