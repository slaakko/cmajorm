#ifndef TEST_HPP_03908BDD6AE840D2388CF6A916D601CB77831F32
#define TEST_HPP_03908BDD6AE840D2388CF6A916D601CB77831F32
#include <sngxml/serialization/XmlPtr.hpp>
#include <soulng/util/Time.hpp>
#include <chrono>
#include <memory>
#include <stdint.h>

using date = soulng::util::Date;
using datetime = soulng::util::DateTime;
using timestamp = soulng::util::Timestamp;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;
using uuid = boost::uuids::uuid;
template<class T> using xml_ptr = sngxml::xmlser::XmlPtr<T>;
template<class T> using unique_xml_ptr = sngxml::xmlser::UniqueXmlPtr<T>;

class SimpleClass : public sngxml::xmlser::XmlSerializable
{
public:
    SimpleClass();
    virtual ~SimpleClass();
    static void* Create();
    static std::string StaticClassName();
    static void Register(int classId_);
    void DestroyObject() override { delete this; }
    const boost::uuids::uuid& ObjectId() const override { return objectId; }
    void SetObjectId(const boost::uuids::uuid& objectId_) override { objectId = objectId_; }
    int ClassId() const override { return classId; }
    bool IsOwned() const override { return isOwned; }
    void SetOwned() override { isOwned = true; }
    void ResetOwned() override { isOwned = false; }
    std::string ClassName() const override;
    sngxml::xmlser::XmlContainer* Container() const override { return container; }
    void SetContainer(sngxml::xmlser::XmlContainer* container_) override { container = container_; }
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    std::string member;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class BaseClass : public sngxml::xmlser::XmlSerializable
{
public:
    BaseClass();
    virtual ~BaseClass();
    static void* Create();
    static std::string StaticClassName();
    static void Register(int classId_);
    void DestroyObject() override { delete this; }
    const boost::uuids::uuid& ObjectId() const override { return objectId; }
    void SetObjectId(const boost::uuids::uuid& objectId_) override { objectId = objectId_; }
    int ClassId() const override { return classId; }
    bool IsOwned() const override { return isOwned; }
    void SetOwned() override { isOwned = true; }
    void ResetOwned() override { isOwned = false; }
    std::string ClassName() const override;
    sngxml::xmlser::XmlContainer* Container() const override { return container; }
    void SetContainer(sngxml::xmlser::XmlContainer* container_) override { container = container_; }
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    std::string member;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class DerivedClass : public BaseClass
{
public:
    DerivedClass();
    virtual ~DerivedClass();
    static void* Create();
    static std::string StaticClassName();
    static void Register(int classId_);
    void DestroyObject() override { delete this; }
    int ClassId() const override { return classId; }
    std::string ClassName() const override;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    void SetObjectXmlAttributes(sngxml::dom::Element* element) const override;
public:
    std::string derivedMember;
private:
    static int classId;
};

class XmlTestClass : public sngxml::xmlser::XmlSerializable
{
public:
    XmlTestClass();
    XmlTestClass(const XmlTestClass&) = delete;
    XmlTestClass(XmlTestClass&&) = delete;
    XmlTestClass& operator=(const XmlTestClass&) = delete;
    XmlTestClass& operator=(XmlTestClass&&) = delete;
    virtual ~XmlTestClass();
    static void* Create();
    static std::string StaticClassName();
    static void Register(int classId_);
    void DestroyObject() override { delete this; }
    const boost::uuids::uuid& ObjectId() const override { return objectId; }
    void SetObjectId(const boost::uuids::uuid& objectId_) override { objectId = objectId_; }
    int ClassId() const override { return classId; }
    bool IsOwned() const override { return isOwned; }
    void SetOwned() override { isOwned = true; }
    void ResetOwned() override { isOwned = false; }
    std::string ClassName() const override;
    sngxml::xmlser::XmlContainer* Container() const override { return container; }
    void SetContainer(sngxml::xmlser::XmlContainer* container_) override { container = container_; }
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
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
    SimpleClass f22;
    std::vector<SimpleClass> f23;
    std::unique_ptr<BaseClass> f24;
    std::unique_ptr<BaseClass> f25;
    std::unique_ptr<BaseClass> f26;
    std::vector<std::unique_ptr<BaseClass>> f27;
    xml_ptr<SimpleClass> f28;
    xml_ptr<BaseClass> f29;
    xml_ptr<BaseClass> f30;
    unique_xml_ptr<SimpleClass> f31;
    unique_xml_ptr<BaseClass> f32;
    unique_xml_ptr<BaseClass> f33;
    std::vector<xml_ptr<BaseClass>> f34;
    std::vector<unique_xml_ptr<BaseClass>> f35;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

#endif // TEST_HPP_03908BDD6AE840D2388CF6A916D601CB77831F32
