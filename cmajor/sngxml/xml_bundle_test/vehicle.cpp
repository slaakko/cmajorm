#include "vehicle.hpp"
#include <sngxml/xml_bundle_test/person.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int Vehicle::classId = -1;

Vehicle::Vehicle()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), color(), owner(), dataPtr(nullptr), isOwned(false)
{
}

void* Vehicle::Create()
{
    return new Vehicle();
}

void Vehicle::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Vehicle>(classId, &Vehicle::Create);
}

std::string Vehicle::StaticClassName()
{
    return "Vehicle";
}

std::string Vehicle::ClassName() const
{
    return "Vehicle";
}

void Vehicle::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "color", color);
    sngxml::xmlser::FromXml(element, "owner", owner);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Vehicle::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    ptrs.push_back(const_cast<xml_ptr<Person>*>(&owner)); 
    return ptrs;
}

void Vehicle::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Vehicle::~Vehicle()
{
}

std::unique_ptr<sngxml::dom::Element> Vehicle::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))
    {
        SetObjectXmlAttributes(element.get());
    }
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(color, "color", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(owner, "owner", ctx).release()));
    return element;
}

