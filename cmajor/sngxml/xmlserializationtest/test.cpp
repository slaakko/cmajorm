#include "test.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

Nonpolymorphic::Nonpolymorphic()
    : member()
{
}

Nonpolymorphic::Nonpolymorphic(sngxml::dom::Element* element)
    : member()
{
    sngxml::xmlser::FromXml(element, "member", member);
}

std::unique_ptr<sngxml::dom::Element> Nonpolymorphic::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(member, "member").release()));
    return element;
}

Polymorphic::Polymorphic()
    : member()
{
}

Polymorphic::Polymorphic(sngxml::dom::Element* element)
    : member()
{
    sngxml::xmlser::FromXml(element, "member", member);
}

Polymorphic::~Polymorphic()
{
}

std::unique_ptr<sngxml::dom::Element> Polymorphic::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(member, "member").release()));
    return element;
}

DerivedClass::DerivedClass()
    : Polymorphic()
    , derivedMember()
{
}

DerivedClass::DerivedClass(sngxml::dom::Element* element)
    : Polymorphic(sngxml::xmlser::GetXmlFieldElement("base", element))
    , derivedMember()
{
    sngxml::xmlser::FromXml(element, "derivedMember", derivedMember);
}

std::unique_ptr<sngxml::dom::Element> DerivedClass::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(Polymorphic::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(derivedMember, "derivedMember").release()));
    return element;
}

XmlTestClass::XmlTestClass()
    : f0(), f1(), f2(), f3(), f4(), f5(), f6(), f7(), f8(), f9(), f10(), f11(), f12(), f13(), f14(), f15(), ts(), time(), f16(), f17(), f18(), f19(), f20(), f21(), f22(), f23(), f24(), f25(), f26(), f27()
{
}

XmlTestClass::XmlTestClass(sngxml::dom::Element* element)
    : f0(), f1(), f2(), f3(), f4(), f5(), f6(), f7(), f8(), f9(), f10(), f11(), f12(), f13(), f14(), f15(), ts(), time(), f16(), f17(), f18(), f19(), f20(), f21(), f22(), f23(), f24(), f25(), f26(), f27()
{
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
}

std::unique_ptr<sngxml::dom::Element> XmlTestClass::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f0, "f0").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f1, "f1").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f2, "f2").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f3, "f3").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f4, "f4").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f5, "f5").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f6, "f6").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f7, "f7").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f8, "f8").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f9, "f9").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f10, "f10").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f11, "f11").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f12, "f12").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f13, "f13").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f14, "f14").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f15, "f15").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ts, "ts").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(time, "time").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f16, "f16").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f17, "f17").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f18, "f18").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f19, "f19").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f20, "f20").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f21, "f21").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f22, "f22").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f23, "f23").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f24, "f24").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f25, "f25").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f26, "f26").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(f27, "f27").release()));
    return element;
}

