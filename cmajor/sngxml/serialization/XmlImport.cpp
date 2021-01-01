// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlImport.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace sngxml { namespace xmlser {

using namespace soulng::unicode;

bool ParseBool(const std::string& str)
{
    if (str == "true") return true;
    if (str == "false") return false;
    throw std::runtime_error("cannot parse Boolean from string '" + str + "'");
}

inline int8_t ParseSByte(const std::string& str)
{
    return static_cast<int8_t>(boost::lexical_cast<int>(str));
}

inline uint8_t ParseByte(const std::string& str)
{
    return static_cast<int8_t>(boost::lexical_cast<int>(str));
}

inline uint8_t ParseHexByte(const std::string& s)
{
    return soulng::util::ParseHexByte(s);
}

inline int16_t ParseShort(const std::string& str)
{
    return boost::lexical_cast<int16_t>(str);
}

inline uint16_t ParseUShort(const std::string& str)
{
    return boost::lexical_cast<uint16_t>(str);
}

inline int32_t ParseInt(const std::string& str)
{
    return boost::lexical_cast<int32_t>(str);
}

inline uint32_t ParseUInt(const std::string& str)
{
    return boost::lexical_cast<uint32_t>(str);
}

inline int64_t ParseLong(const std::string& str)
{
    return boost::lexical_cast<int64_t>(str);
}

inline uint64_t ParseULong(const std::string& str)
{
    return boost::lexical_cast<uint64_t>(str);
}

inline float ParseFloat(const std::string& str)
{
    return boost::lexical_cast<float>(str);
}

inline double ParseDouble(const std::string& str)
{
    return boost::lexical_cast<double>(str);
}

char ParseChar(const std::string& str)
{
    if (str.empty()) throw std::runtime_error("cannot parse char from string '" + str + "'");
    return str[0];
}

char16_t ParseWChar(const std::string& str)
{
    if (str.empty()) throw std::runtime_error("cannot parse wchar from string '" + str + "'");
    return ToUtf16(str)[0];
}

char32_t ParseUChar(const std::string& str)
{
    if (str.empty()) throw std::runtime_error("cannot parse uchar from string '" + str + "'");
    return ToUtf32(str)[0];
}

boost::uuids::uuid ParseUuid(const std::string& str)
{
    if (str.length() != boost::uuids::uuid::static_size() * 2) throw std::runtime_error("cannot parse uuid from string '" + str + "'");
    boost::uuids::uuid x = boost::uuids::nil_uuid();
    for (boost::uuids::uuid::size_type i = 0; i < boost::uuids::uuid::static_size(); ++i)
    {
        std::string s;
        s.append(1, str[2 * i]);
        s.append(1, str[2 * i + 1]);
        x.data[i] = ParseHexByte(s);
    }
    return x;
}

date ParseDate(const std::string& str)
{
    return soulng::util::ParseDate(str);
}

datetime ParseDateTime(const std::string& dateTimeStr)
{
    return soulng::util::ParseDateTime(dateTimeStr);
}

sngxml::dom::Element* GetXmlFieldElement(const std::string& fieldName, sngxml::dom::Element* fromElement)
{
    std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(ToUtf32(fieldName), fromElement);
    if (result)
    {
        if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
            if (nodeSet->Length() == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* fieldElement = static_cast<sngxml::dom::Element*>(node);
                    return fieldElement;
                }
            }
        }
    }
    return nullptr;
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, bool& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseBool(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int8_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseSByte(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint8_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseByte(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int16_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseShort(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint16_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseUShort(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int32_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseInt(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint32_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseUInt(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int64_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseLong(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint64_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseULong(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, float& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseFloat(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, double& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseDouble(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseChar(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char16_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseWChar(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char32_t& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseUChar(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::string& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ToUtf8(element->GetAttribute(U"value"));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::u16string& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ToUtf16(element->GetAttribute(U"value"));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::u32string& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = element->GetAttribute(U"value");
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uuid& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseUuid(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, date& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseDate(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, datetime& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = ParseDateTime(ToUtf8(element->GetAttribute(U"value")));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, time_point& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = time_point(duration(ParseLong(ToUtf8(element->GetAttribute(U"value")))));
    }
}

void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, duration& value)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        value = duration(ParseLong(ToUtf8(element->GetAttribute(U"value"))));
    }
}

} } // namespace sngxml::xmlser
