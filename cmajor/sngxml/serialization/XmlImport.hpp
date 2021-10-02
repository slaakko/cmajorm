// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_IMPORT_INCLUDED
#define SNGXML_XML_IMPORT_INCLUDED
#include <sngxml/serialization/XmlClassRegistry.hpp>
#include <sngxml/serialization/XmlPtr.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Time.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/lexical_cast.hpp>
#include <chrono>

namespace sngxml { namespace xmlser {

using date = soulng::util::Date;
using datetime = soulng::util::DateTime;
using timestamp = soulng::util::Timestamp;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;
using uuid = boost::uuids::uuid;

template<class T>
concept XmlImportable = 
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
concept XmlImportableEnumType = std::is_enum_v<T>;

template<class T>
concept XmlConstructible = std::is_class_v<T> && requires(T t, sngxml::dom::Element* element)
{ 
    T();
    t.FromXml(element);
};

SNGXML_SERIALIZATION_API sngxml::dom::Element* GetXmlFieldElement(const std::string& fieldName, sngxml::dom::Element* fromElement);

SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, bool& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, bool& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, int8_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int8_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, uint8_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint8_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, int16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, uint16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, int32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, uint32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, int64_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, int64_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, uint64_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uint64_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, float& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, float& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, double& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, double& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, char& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, char16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char16_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, char32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, char32_t& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, std::string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, std::u16string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::u16string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, std::u32string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::u32string& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, uuid& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, uuid& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, date& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, date& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, datetime& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, datetime& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, timestamp& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, timestamp& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, time_point& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, time_point& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* element, duration& value);
SNGXML_SERIALIZATION_API void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, duration& value);

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, T& object)
{
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        object.FromXml(element);
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
            std::u32string classIdAttr = element->GetAttribute(U"classId");
            if (!classIdAttr.empty())
            {
                int classId = boost::lexical_cast<int>(ToUtf8(classIdAttr));
                object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(classId)));
                object->FromXml(element);
            }
        }
    }
}

template<XmlImportableEnumType T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, T& value)
{
    value = T();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        std::u32string val = element->GetAttribute(U"value");
        value = static_cast<T>(boost::lexical_cast<int64_t>(ToUtf8(val)));
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
            std::u32string classIdAttr = element->GetAttribute(U"classId");
            if (!classIdAttr.empty())
            {
                int classId = boost::lexical_cast<int>(ToUtf8(classIdAttr));
                object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(classId)));
                object->FromXml(element);
            }
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, sngxml::xmlser::XmlPtr<T>& xmlPtr)
{
    xmlPtr.Reset();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        std::u32string value = element->GetAttribute(U"value");
        if (value != U"null")
        {
            std::u32string objectIdAttr = element->GetAttribute(U"objectId");
            if (!objectIdAttr.empty())
            {
                boost::uuids::uuid objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
                xmlPtr.SetTargetObjectId(objectId);
            }
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, sngxml::xmlser::UniqueXmlPtr<T>& xmlPtr)
{
    xmlPtr.Reset();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
        std::u32string value = element->GetAttribute(U"value");
        if (value != U"null")
        {
            std::u32string objectIdAttr = element->GetAttribute(U"objectId");
            if (!objectIdAttr.empty())
            {
                boost::uuids::uuid objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
                xmlPtr.SetTargetObjectId(objectId);
            }
        }
    }
}

template<XmlImportable T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::vector<T>& v)
{
    v.clear();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
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
                        FromXml(element, value);
                        v.push_back(std::move(value));
                    }
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
    if (element)
    {
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
                        T object;
                        object.FromXml(element);
                        v.push_back(std::move(object));
                    }
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
    if (element)
    {
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
                            std::u32string classIdAttr = element->GetAttribute(U"classId");
                            if (!classIdAttr.empty())
                            {
                                int classId = boost::lexical_cast<int>(ToUtf8(classIdAttr));
                                object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(classId)));
                                object->FromXml(element);
                            }
                        }
                        v.push_back(std::move(object));
                    }
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
    if (element)
    {
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
                            std::u32string classIdAttr = element->GetAttribute(U"classId");
                            if (!classIdAttr.empty())
                            {
                                int classId = boost::lexical_cast<int>(ToUtf8(classIdAttr));
                                object.reset(static_cast<T*>(XmlClassRegistry::Instance().Create(classId)));
                                object->FromXml(element);
                            }
                        }
                        v.push_back(std::move(object));
                    }
                }
            }
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::vector<sngxml::xmlser::XmlPtr<T>>& list)
{
    list.clear();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
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
                        list.push_back(sngxml::xmlser::XmlPtr<T>());
                        sngxml::xmlser::XmlPtr<T>& ptr = list.back();
                        std::u32string value = element->GetAttribute(U"value");
                        if (value != U"null")
                        {
                            std::u32string objectIdAttr = element->GetAttribute(U"objectId");
                            if (!objectIdAttr.empty())
                            {
                                boost::uuids::uuid objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
                                ptr.SetTargetObjectId(objectId);
                            }
                        }
                    }
                }
            }
        }
    }
}

template<XmlConstructible T>
void FromXml(sngxml::dom::Element* parentElement, const std::string& fieldName, std::vector<sngxml::xmlser::UniqueXmlPtr<T>>& list)
{
    list.clear();
    sngxml::dom::Element* element = GetXmlFieldElement(fieldName, parentElement);
    if (element)
    {
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
                        list.push_back(sngxml::xmlser::UniqueXmlPtr<T>());
                        sngxml::xmlser::UniqueXmlPtr<T>& ptr = list.back();
                        std::u32string value = element->GetAttribute(U"value");
                        if (value != U"null")
                        {
                            std::u32string objectIdAttr = element->GetAttribute(U"objectId");
                            if (!objectIdAttr.empty())
                            {
                                boost::uuids::uuid objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
                                ptr.SetTargetObjectId(objectId);
                            }
                        }
                    }
                }
            }
        }
    }
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_IMPORT_INCLUDED
