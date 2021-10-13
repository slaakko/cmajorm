#include "car.hpp"
#include <sngxml/xml_bundle_test/person.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int Car::classId = -1;

Car::Car()
     : Vehicle(), registrationNumber()
{
}

void* Car::Create()
{
    return new Car();
}

void Car::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Car>(classId, &Car::Create);
}

std::string Car::StaticClassName()
{
    return "Car";
}

std::string Car::ClassName() const
{
    return "Car";
}

void Car::FromXml(sngxml::dom::Element* element)
{
    Vehicle::FromXml(element);
    sngxml::xmlser::FromXml(element, "registrationNumber", registrationNumber);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Car::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = Vehicle::GetPtrs();
    return ptrs;
}

void Car::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Car::~Car()
{
}

std::unique_ptr<sngxml::dom::Element> Car::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element = Vehicle::ToXml(fieldName, ctx);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(registrationNumber, "registrationNumber", ctx).release()));
    return element;
}

