#ifndef SOLUTIONSETTINGS_HPP_CBFFFB300F3FB8B5357239390EFF01476489416B
#define SOLUTIONSETTINGS_HPP_CBFFFB300F3FB8B5357239390EFF01476489416B
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

class SolutionBreakpoint : public sngxml::xmlser::XmlSerializable
{
public:
    SolutionBreakpoint();
    virtual ~SolutionBreakpoint();
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
    std::string file;
    int32_t line;
    std::string condition;
    bool disabled;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

class SolutionSettings : public sngxml::xmlser::XmlSerializable
{
public:
    SolutionSettings();
    virtual ~SolutionSettings();
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
    bool callStackOpen;
    bool localsViewOpen;
    std::vector<std::string> openFiles;
    std::string currentOpenFile;
    int32_t currentCursorLine;
    std::vector<SolutionBreakpoint> breakpoints;
    std::vector<std::string> expandedProjects;
    void* dataPtr;
private:
    static int classId;
    boost::uuids::uuid objectId;
    sngxml::xmlser::XmlContainer* container;
    bool isOwned;
};

#endif // SOLUTIONSETTINGS_HPP_CBFFFB300F3FB8B5357239390EFF01476489416B
