#ifndef VEHICLE_HPP_87AA9A703BD3AEEFE2F3DDC4DF3BD1DE137171E6
#define VEHICLE_HPP_87AA9A703BD3AEEFE2F3DDC4DF3BD1DE137171E6
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

enum class VehicleColor
{
    red
    , green
    , blue
    , black
    , white
};

class Person;

class Vehicle : public sngxml::xmlser::XmlSerializable
{
public:
    Vehicle();
    Vehicle(const Vehicle&) = delete;
    Vehicle(Vehicle&&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;
    Vehicle& operator=(Vehicle&&) = delete;
    virtual ~Vehicle();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    VehicleColor color;
    Person* GetOwner() const
    {
        return owner.Get();
    }
    void SetOwner(Person* owner_)
    {
        owner = owner_;
    }
    xml_ptr<Person> owner;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

#endif // VEHICLE_HPP_87AA9A703BD3AEEFE2F3DDC4DF3BD1DE137171E6
