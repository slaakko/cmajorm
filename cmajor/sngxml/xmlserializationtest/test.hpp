#ifndef TEST_HPP_03908BDD6AE840D2388CF6A916D601CB77831F32
#define TEST_HPP_03908BDD6AE840D2388CF6A916D601CB77831F32
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Time.hpp>
#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <stdint.h>

using date = soulng::util::Date;
using datetime = soulng::util::DateTime;
using timestamp = soulng::util::Timestamp;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;
using uuid = boost::uuids::uuid;

class Nonpolymorphic
{
public:
    Nonpolymorphic();
    Nonpolymorphic(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string member;
};

class Polymorphic
{
public:
    Polymorphic();
    Polymorphic(sngxml::dom::Element* element);
    virtual ~Polymorphic();
    virtual std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string member;
};

class DerivedClass : public Polymorphic
{
public:
    DerivedClass();
    DerivedClass(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    std::string derivedMember;
};

class XmlTestClass
{
public:
    XmlTestClass();
    XmlTestClass(sngxml::dom::Element* element);
    XmlTestClass(const XmlTestClass&) = delete;
    XmlTestClass(XmlTestClass&&) = delete;
    XmlTestClass& operator=(const XmlTestClass&) = delete;
    XmlTestClass& operator=(XmlTestClass&&) = delete;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool f0;
    int8_t f1;
    uint8_t f2;
    int16_t f3;
    uint16_t f4;
    int32_t f5;
    uint32_t f6;
    int64_t f7;
    uint64_t f8;
    float f9;
    double f10;
    char f11;
    char16_t f12;
    char32_t f13;
    date f14;
    datetime f15;
    timestamp ts;
    int64_t time;
    std::string f16;
    std::u16string f17;
    std::u32string f18;
    time_point f19;
    duration f20;
    uuid f21;
    Nonpolymorphic f22;
    std::vector<Nonpolymorphic> f23;
    std::unique_ptr<Polymorphic> f24;
    std::unique_ptr<Polymorphic> f25;
    std::unique_ptr<Polymorphic> f26;
    std::vector<std::unique_ptr<Polymorphic>> f27;
};

#endif // TEST_HPP_03908BDD6AE840D2388CF6A916D601CB77831F32
