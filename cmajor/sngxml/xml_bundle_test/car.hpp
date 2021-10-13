#ifndef CAR_HPP_8D1C7C231EA09BAB2D23922DD17EAA107C2DA650
#define CAR_HPP_8D1C7C231EA09BAB2D23922DD17EAA107C2DA650
#include <sngxml/xml_bundle_test/vehicle.hpp>
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

class Car : public Vehicle
{
public:
    Car();
    virtual ~Car();
    static void* Create();
    static std::string StaticClassName();
    static void Register(int classId_);
    void DestroyObject() override { delete this; }
    int ClassId() const override { return classId; }
    std::string ClassName() const override;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    void SetObjectXmlAttributes(sngxml::dom::Element* element) const override;
public:
    std::string registrationNumber;
private:
    static int classId;
};

#endif // CAR_HPP_8D1C7C231EA09BAB2D23922DD17EAA107C2DA650
