#include "person.hpp"
#include <sngxml/xml_bundle_test/vehicle.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int Person::classId = -1;

Person::Person()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), name(), age(), vehicles(), dataPtr(nullptr), isOwned(false)
{
}

void* Person::Create()
{
    return new Person();
}

void Person::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Person>(classId, &Person::Create);
}

std::string Person::StaticClassName()
{
    return "Person";
}

std::string Person::ClassName() const
{
    return "Person";
}

void Person::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "name", name);
    sngxml::xmlser::FromXml(element, "age", age);
    sngxml::xmlser::FromXml(element, "vehicles", vehicles);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Person::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    for (const auto& ptr : vehicles)
    {
        ptrs.push_back(const_cast<unique_xml_ptr<Vehicle>*>(&ptr));
    }
    return ptrs;
}

void Person::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Person::~Person()
{
}

std::unique_ptr<sngxml::dom::Element> Person::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(name, "name").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(age, "age").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(vehicles, "vehicles").release()));
    return element;
}

void Person::AddVehicle(Vehicle* v)
{
    v->SetOwner(this);
    vehicles.push_back(unique_xml_ptr<Vehicle>(v));
}
