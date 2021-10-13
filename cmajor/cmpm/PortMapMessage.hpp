#ifndef PORTMAPMESSAGE_HPP_DC8E9733F60B35A671F1CC8A29699D85F1D0FE2B
#define PORTMAPMESSAGE_HPP_DC8E9733F60B35A671F1CC8A29699D85F1D0FE2B
#include <cmajor/cmpm/Api.hpp>
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

class CMPM_API HelloPmsRequest : public sngxml::xmlser::XmlSerializable
{
public:
    HelloPmsRequest();
    virtual ~HelloPmsRequest();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API HelloPmsReply : public sngxml::xmlser::XmlSerializable
{
public:
    HelloPmsReply();
    virtual ~HelloPmsReply();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    std::string version;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API GetFreePortNumberRequest : public sngxml::xmlser::XmlSerializable
{
public:
    GetFreePortNumberRequest();
    virtual ~GetFreePortNumberRequest();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    std::string processName;
    int32_t pid;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API GetFreePortNumberReply : public sngxml::xmlser::XmlSerializable
{
public:
    GetFreePortNumberReply();
    virtual ~GetFreePortNumberReply();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    int32_t portNumber;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API ExtendPortLeaseRequest : public sngxml::xmlser::XmlSerializable
{
public:
    ExtendPortLeaseRequest();
    virtual ~ExtendPortLeaseRequest();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    std::string processName;
    int32_t pid;
    std::vector<int32_t> portNumbers;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API ExtendPortLeaseReply : public sngxml::xmlser::XmlSerializable
{
public:
    ExtendPortLeaseReply();
    virtual ~ExtendPortLeaseReply();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API StopPortMapServerRequest : public sngxml::xmlser::XmlSerializable
{
public:
    StopPortMapServerRequest();
    virtual ~StopPortMapServerRequest();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API StopPortMapServerReply : public sngxml::xmlser::XmlSerializable
{
public:
    StopPortMapServerReply();
    virtual ~StopPortMapServerReply();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API PortLease : public sngxml::xmlser::XmlSerializable
{
public:
    PortLease();
    virtual ~PortLease();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    int32_t portNumber;
    std::string processName;
    int32_t pid;
    datetime leaseStartTime;
    datetime leaseRenewalTime;
    std::string leaseState;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API ViewPortLeaseRequest : public sngxml::xmlser::XmlSerializable
{
public:
    ViewPortLeaseRequest();
    virtual ~ViewPortLeaseRequest();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class CMPM_API ViewPortLeaseReply : public sngxml::xmlser::XmlSerializable
{
public:
    ViewPortLeaseReply();
    virtual ~ViewPortLeaseReply();
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
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;
    void FromXml(sngxml::dom::Element* element) override;
    std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;
    virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;
public:
    std::vector<PortLease> portLeases;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

#endif // PORTMAPMESSAGE_HPP_DC8E9733F60B35A671F1CC8A29699D85F1D0FE2B
