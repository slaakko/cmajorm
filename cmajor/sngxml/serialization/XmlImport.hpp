// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_IMPORT_INCLUDED
#define SNGXML_XML_IMPORT_INCLUDED
#include <sngxml/serialization/XmlClassRegistry.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Time.hpp>
#include <boost/uuid/uuid.hpp>
#include <chrono>

namespace sngxml { namespace xmlser {

using date = soulng::util::Date;
using datetime = soulng::util::DateTime;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;
using uuid = boost::uuids::uuid;

template<class T>
concept XmlConstructible = requires(sngxml::dom::Element* element) { T(element); };

template<class T>
concept XmlImportableFundamentalType = std::is_fundamental_v<T>;

template<class T>
concept XmlImportable = 
requires { XmlImportableFundamentalType<T>; } || requires { XmlConstructible<T>;  };

SNGXML_SERIALIZATION_API sngxml::dom::Element* GetXmlFieldElement(const std::string& fieldName, sngxml::dom::Element* fromElement);

SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, bool& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int8_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint8_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int64_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint64_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, float& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, double& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::u16string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::u32string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uuid& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, date& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, datetime& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, time_point& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, duration& value);

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, T& object)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        object = T(element);
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::unique_ptr<T>& object)
{
    object.reset();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        std::u32string value = element->GetAttribute(U"value");
        if (value != U"null")
        {
            object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(element)));
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::shared_ptr<T>& object)
{
    object.reset();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        std::u32string value = element->GetAttribute(U"value");
        if (value != U"null")
        {
            object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(element)));
        }
    }
}

template<XmlImportableFundamentalType T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::vector<T>& v)
{
    v.clear();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(U"item", element);
    if (result)
    {
        if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    T value;
                    FromXml(element, "value", value);
                    v.push_back(std::move(value));
                }
            }
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::vector<T>& v)
{
    v.clear();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(U"item", element);
    if (result)
    {
        if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    T object(element);
                    v.push_back(std::move(object));
                }
            }
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::vector<std::unique_ptr<T>>& v)
{
    v.clear();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(U"item", element);
    if (result)
    {
        if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::unique_ptr<T> object;
                    std::u32string value = element->GetAttribute(U"value");
                    if (value != U"null")
                    {
                        object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(element)));
                    }
                    v.push_back(std::move(object));
                }
            }
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::vector<std::shared_ptr<T>>& v)
{
    v.clear();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(U"item", element);
    if (result)
    {
        if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::shared_ptr<T> object;
                    std::u32string value = element->GetAttribute(U"value");
                    if (value != U"null")
                    {
                        object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(element)));
                    }
                    v.push_back(std::move(object));
                }
            }
        }
    }
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_IMPORT_INCLUDED
