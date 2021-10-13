#include "bicycle.hpp"
#include <sngxml/xml_bundle_test/person.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int Bicycle::classId = -1;

Bicycle::Bicycle()
     : Vehicle(), price()
{
}

void* Bicycle::Create()
{
    return new Bicycle();
}

void Bicycle::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Bicycle>(classId, &Bicycle::Create);
}

std::string Bicycle::StaticClassName()
{
    return "Bicycle";
}

std::string Bicycle::ClassName() const
{
    return "Bicycle";
}

void Bicycle::FromXml(sngxml::dom::Element* element)
{
    Vehicle::FromXml(element);
    sngxml::xmlser::FromXml(element, "price", price);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Bicycle::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = Vehicle::GetPtrs();
    return ptrs;
}

void Bicycle::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Bicycle::~Bicycle()
{
}

std::unique_ptr<sngxml::dom::Element> Bicycle::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element = Vehicle::ToXml(fieldName, ctx);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(price, "price", ctx).release()));
    return element;
}

