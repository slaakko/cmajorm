// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_SERIALIZABLE_INCLUDED
#define SNGXML_XML_SERIALIZABLE_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>
#include <sngxml/dom/Element.hpp>
#include <boost/uuid/uuid.hpp>
#include <string>
#include <vector>

namespace sngxml { namespace xmlser {

class XmlContainer;
class XmlPtrBase;

class SNGXML_SERIALIZATION_API XmlSerializable
{
public:
    virtual ~XmlSerializable();
    virtual void DestroyObject() = 0;
    virtual const boost::uuids::uuid& ObjectId() const = 0;
    virtual void SetObjectId(const boost::uuids::uuid& objectId_) = 0;
    virtual int ClassId() const = 0;
    virtual std::string ClassName() const = 0;
    virtual sngxml::xmlser::XmlContainer* Container() const = 0;
    virtual void SetContainer(sngxml::xmlser::XmlContainer* container_) = 0;
    virtual void FromXml(sngxml::dom::Element* element) = 0;
    virtual std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& elementName) const = 0;
    virtual std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const = 0;
    void SetRandomObjectId();
};

template<class T>
T* XmlCast(XmlSerializable* serializable)
{
    if (serializable)
    {
        if (T::StaticClassName() == serializable->ClassName())
        {
            return static_cast<T*>(serializable);
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_SERIALIZABLE_INCLUDED
