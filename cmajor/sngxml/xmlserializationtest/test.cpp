#include "test.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int SimpleClass::classId = -1;

SimpleClass::SimpleClass()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), member(), dataPtr(nullptr), isOwned(false)
{
}

void* SimpleClass::Create()
{
    return new SimpleClass();
}

void SimpleClass::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<SimpleClass>(classId, &SimpleClass::Create);
}

std::string SimpleClass::StaticClassName()
{
    return "SimpleClass";
}

std::string SimpleClass::ClassName() const
{
    return "SimpleClass";
}

void SimpleClass::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "member", member);
}

std::vector<sngxml::xmlser::XmlPtrBase*> SimpleClass::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void SimpleClass::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

SimpleClass::~SimpleClass()
{
}

std::unique_ptr<sngxml::dom::Element> SimpleClass::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> SimpleClass::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))
    {
        SetObjectXmlAttributes(element.get());
    }
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(member, "member", ctx).release()));
    return element;
}

int BaseClass::classId = -1;

BaseClass::BaseClass()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), member(), dataPtr(nullptr), isOwned(false)
{
}

void* BaseClass::Create()
{
    return new BaseClass();
}

void BaseClass::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BaseClass>(classId, &BaseClass::Create);
}

std::string BaseClass::StaticClassName()
{
    return "BaseClass";
}

std::string BaseClass::ClassName() const
{
    return "BaseClass";
}

void BaseClass::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "member", member);
}

std::vector<sngxml::xmlser::XmlPtrBase*> BaseClass::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BaseClass::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BaseClass::~BaseClass()
{
}

std::unique_ptr<sngxml::dom::Element> BaseClass::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> BaseClass::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))
    {
        SetObjectXmlAttributes(element.get());
    }
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(member, "member", ctx).release()));
    return element;
}

int DerivedClass::classId = -1;

DerivedClass::DerivedClass()
     : BaseClass(), derivedMember()
{
}

void* DerivedClass::Create()
{
    return new DerivedClass();
}

void DerivedClass::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<DerivedClass>(classId, &DerivedClass::Create);
}

std::string DerivedClass::StaticClassName()
{
    return "DerivedClass";
}

std::string DerivedClass::ClassName() const
{
    return "DerivedClass";
}

void DerivedClass::FromXml(sngxml::dom::Element* element)
{
    BaseClass::FromXml(element);
    sngxml::xmlser::FromXml(element, "derivedMember", derivedMember);
}

std::vector<sngxml::xmlser::XmlPtrBase*> DerivedClass::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = BaseClass::GetPtrs();
    return ptrs;
}

void DerivedClass::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

DerivedClass::~DerivedClass()
{
}

std::unique_ptr<sngxml::dom::Element> DerivedClass::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> DerivedClass::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element = BaseClass::ToXml(fieldName, ctx);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(derivedMember, "derivedMember", ctx).release()));
    return element;
}

int XmlTestClass::classId = -1;

XmlTestClass::XmlTestClass()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), f0(), f1(), f2(), f3(), f4(), f5(), f6(), f7(), f8(), f9(), f10(), f11(), f12(), f13(), f14(), f15(), ts(), time(), f16(), f17(), f18(), f19(), f20(), f21(), f22(), f23(), f24(), f25(), f26(), f27(), f28(), f29(), f30(), f31(), f32(), f33(), f34(), f35(), dataPtr(nullptr), isOwned(false)
{
}

void* XmlTestClass::Create()
{
    return new XmlTestClass();
}

void XmlTestClass::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<XmlTestClass>(classId, &XmlTestClass::Create);
}

std::string XmlTestClass::StaticClassName()
{
    return "XmlTestClass";
}

std::string XmlTestClass::ClassName() const
{
    return "XmlTestClass";
}

void XmlTestClass::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "f0", f0);
    sngxml::xmlser::FromXml(element, "f1", f1);
    sngxml::xmlser::FromXml(element, "f2", f2);
    sngxml::xmlser::FromXml(element, "f3", f3);
    sngxml::xmlser::FromXml(element, "f4", f4);
    sngxml::xmlser::FromXml(element, "f5", f5);
    sngxml::xmlser::FromXml(element, "f6", f6);
    sngxml::xmlser::FromXml(element, "f7", f7);
    sngxml::xmlser::FromXml(element, "f8", f8);
    sngxml::xmlser::FromXml(element, "f9", f9);
    sngxml::xmlser::FromXml(element, "f10", f10);
    sngxml::xmlser::FromXml(element, "f11", f11);
    sngxml::xmlser::FromXml(element, "f12", f12);
    sngxml::xmlser::FromXml(element, "f13", f13);
    sngxml::xmlser::FromXml(element, "f14", f14);
    sngxml::xmlser::FromXml(element, "f15", f15);
    sngxml::xmlser::FromXml(element, "ts", ts);
    sngxml::xmlser::FromXml(element, "time", time);
    sngxml::xmlser::FromXml(element, "f16", f16);
    sngxml::xmlser::FromXml(element, "f17", f17);
    sngxml::xmlser::FromXml(element, "f18", f18);
    sngxml::xmlser::FromXml(element, "f19", f19);
    sngxml::xmlser::FromXml(element, "f20", f20);
    sngxml::xmlser::FromXml(element, "f21", f21);
    sngxml::xmlser::FromXml(element, "f22", f22);
    sngxml::xmlser::FromXml(element, "f23", f23);
    sngxml::xmlser::FromXml(element, "f24", f24);
    sngxml::xmlser::FromXml(element, "f25", f25);
    sngxml::xmlser::FromXml(element, "f26", f26);
    sngxml::xmlser::FromXml(element, "f27", f27);
    sngxml::xmlser::FromXml(element, "f28", f28);
    sngxml::xmlser::FromXml(element, "f29", f29);
    sngxml::xmlser::FromXml(element, "f30", f30);
    sngxml::xmlser::FromXml(element, "f31", f31);
    sngxml::xmlser::FromXml(element, "f32", f32);
    sngxml::xmlser::FromXml(element, "f33", f33);
    sngxml::xmlser::FromXml(element, "f34", f34);
    sngxml::xmlser::FromXml(element, "f35", f35);
}

std::vector<sngxml::xmlser::XmlPtrBase*> XmlTestClass::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    ptrs.push_back(const_cast<xml_ptr<SimpleClass>*>(&f28)); 
    ptrs.push_back(const_cast<xml_ptr<BaseClass>*>(&f29)); 
    ptrs.push_back(const_cast<xml_ptr<BaseClass>*>(&f30)); 
    ptrs.push_back(const_cast<unique_xml_ptr<SimpleClass>*>(&f31)); 
    ptrs.push_back(const_cast<unique_xml_ptr<BaseClass>*>(&f32)); 
    ptrs.push_back(const_cast<unique_xml_ptr<BaseClass>*>(&f33)); 
    for (const auto& ptr : f34)
    {
        ptrs.push_back(const_cast<xml_ptr<BaseClass>*>(&ptr));
    }
    for (const auto& ptr : f35)
    {
        ptrs.push_back(const_cast<unique_xml_ptr<BaseClass>*>(&ptr));
    }
    return ptrs;
}

void XmlTestClass::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

XmlTestClass::~XmlTestClass()
{
}

std::unique_ptr<sngxml::dom::Element> XmlTestClass::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> XmlTestClass::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))
    {
        SetObjectXmlAttributes(element.get());
    }
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f0, "f0", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f1, "f1", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f2, "f2", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f3, "f3", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f4, "f4", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f5, "f5", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f6, "f6", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f7, "f7", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f8, "f8", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f9, "f9", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f10, "f10", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f11, "f11", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f12, "f12", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f13, "f13", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f14, "f14", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f15, "f15", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ts, "ts", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(time, "time", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f16, "f16", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f17, "f17", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f18, "f18", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f19, "f19", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f20, "f20", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f21, "f21", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f22, "f22", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f23, "f23", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f24, "f24", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f25, "f25", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f26, "f26", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f27, "f27", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f28, "f28", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f29, "f29", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f30, "f30", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f31, "f31", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f32, "f32", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f33, "f33", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f34, "f34", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f35, "f35", ctx).release()));
    return element;
}

