#include "PortMapMessage.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int HelloPmsRequest::classId = -1;

HelloPmsRequest::HelloPmsRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* HelloPmsRequest::Create()
{
    return new HelloPmsRequest();
}

void HelloPmsRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<HelloPmsRequest>(classId, &HelloPmsRequest::Create);
}

std::string HelloPmsRequest::StaticClassName()
{
    return "HelloPmsRequest";
}

std::string HelloPmsRequest::ClassName() const
{
    return "HelloPmsRequest";
}

void HelloPmsRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> HelloPmsRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void HelloPmsRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

HelloPmsRequest::~HelloPmsRequest()
{
}

std::unique_ptr<sngxml::dom::Element> HelloPmsRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int HelloPmsReply::classId = -1;

HelloPmsReply::HelloPmsReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), version(), dataPtr(nullptr)
{
}

void* HelloPmsReply::Create()
{
    return new HelloPmsReply();
}

void HelloPmsReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<HelloPmsReply>(classId, &HelloPmsReply::Create);
}

std::string HelloPmsReply::StaticClassName()
{
    return "HelloPmsReply";
}

std::string HelloPmsReply::ClassName() const
{
    return "HelloPmsReply";
}

void HelloPmsReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "version", version);
}

std::vector<sngxml::xmlser::XmlPtrBase*> HelloPmsReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void HelloPmsReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

HelloPmsReply::~HelloPmsReply()
{
}

std::unique_ptr<sngxml::dom::Element> HelloPmsReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(version, "version").release()));
    return element;
}

int GetFreePortNumberRequest::classId = -1;

GetFreePortNumberRequest::GetFreePortNumberRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), processName(), pid(), dataPtr(nullptr)
{
}

void* GetFreePortNumberRequest::Create()
{
    return new GetFreePortNumberRequest();
}

void GetFreePortNumberRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetFreePortNumberRequest>(classId, &GetFreePortNumberRequest::Create);
}

std::string GetFreePortNumberRequest::StaticClassName()
{
    return "GetFreePortNumberRequest";
}

std::string GetFreePortNumberRequest::ClassName() const
{
    return "GetFreePortNumberRequest";
}

void GetFreePortNumberRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "processName", processName);
    sngxml::xmlser::FromXml(element, "pid", pid);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetFreePortNumberRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void GetFreePortNumberRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetFreePortNumberRequest::~GetFreePortNumberRequest()
{
}

std::unique_ptr<sngxml::dom::Element> GetFreePortNumberRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(processName, "processName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(pid, "pid").release()));
    return element;
}

int GetFreePortNumberReply::classId = -1;

GetFreePortNumberReply::GetFreePortNumberReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), portNumber(), dataPtr(nullptr)
{
}

void* GetFreePortNumberReply::Create()
{
    return new GetFreePortNumberReply();
}

void GetFreePortNumberReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetFreePortNumberReply>(classId, &GetFreePortNumberReply::Create);
}

std::string GetFreePortNumberReply::StaticClassName()
{
    return "GetFreePortNumberReply";
}

std::string GetFreePortNumberReply::ClassName() const
{
    return "GetFreePortNumberReply";
}

void GetFreePortNumberReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "portNumber", portNumber);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetFreePortNumberReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void GetFreePortNumberReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetFreePortNumberReply::~GetFreePortNumberReply()
{
}

std::unique_ptr<sngxml::dom::Element> GetFreePortNumberReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portNumber, "portNumber").release()));
    return element;
}

int ExtendPortLeaseRequest::classId = -1;

ExtendPortLeaseRequest::ExtendPortLeaseRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), processName(), pid(), portNumbers(), dataPtr(nullptr)
{
}

void* ExtendPortLeaseRequest::Create()
{
    return new ExtendPortLeaseRequest();
}

void ExtendPortLeaseRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ExtendPortLeaseRequest>(classId, &ExtendPortLeaseRequest::Create);
}

std::string ExtendPortLeaseRequest::StaticClassName()
{
    return "ExtendPortLeaseRequest";
}

std::string ExtendPortLeaseRequest::ClassName() const
{
    return "ExtendPortLeaseRequest";
}

void ExtendPortLeaseRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "processName", processName);
    sngxml::xmlser::FromXml(element, "pid", pid);
    sngxml::xmlser::FromXml(element, "portNumbers", portNumbers);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ExtendPortLeaseRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ExtendPortLeaseRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ExtendPortLeaseRequest::~ExtendPortLeaseRequest()
{
}

std::unique_ptr<sngxml::dom::Element> ExtendPortLeaseRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(processName, "processName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(pid, "pid").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portNumbers, "portNumbers").release()));
    return element;
}

int ExtendPortLeaseReply::classId = -1;

ExtendPortLeaseReply::ExtendPortLeaseReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* ExtendPortLeaseReply::Create()
{
    return new ExtendPortLeaseReply();
}

void ExtendPortLeaseReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ExtendPortLeaseReply>(classId, &ExtendPortLeaseReply::Create);
}

std::string ExtendPortLeaseReply::StaticClassName()
{
    return "ExtendPortLeaseReply";
}

std::string ExtendPortLeaseReply::ClassName() const
{
    return "ExtendPortLeaseReply";
}

void ExtendPortLeaseReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> ExtendPortLeaseReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ExtendPortLeaseReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ExtendPortLeaseReply::~ExtendPortLeaseReply()
{
}

std::unique_ptr<sngxml::dom::Element> ExtendPortLeaseReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int StopPortMapServerRequest::classId = -1;

StopPortMapServerRequest::StopPortMapServerRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* StopPortMapServerRequest::Create()
{
    return new StopPortMapServerRequest();
}

void StopPortMapServerRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopPortMapServerRequest>(classId, &StopPortMapServerRequest::Create);
}

std::string StopPortMapServerRequest::StaticClassName()
{
    return "StopPortMapServerRequest";
}

std::string StopPortMapServerRequest::ClassName() const
{
    return "StopPortMapServerRequest";
}

void StopPortMapServerRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopPortMapServerRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StopPortMapServerRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopPortMapServerRequest::~StopPortMapServerRequest()
{
}

std::unique_ptr<sngxml::dom::Element> StopPortMapServerRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int StopPortMapServerReply::classId = -1;

StopPortMapServerReply::StopPortMapServerReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* StopPortMapServerReply::Create()
{
    return new StopPortMapServerReply();
}

void StopPortMapServerReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopPortMapServerReply>(classId, &StopPortMapServerReply::Create);
}

std::string StopPortMapServerReply::StaticClassName()
{
    return "StopPortMapServerReply";
}

std::string StopPortMapServerReply::ClassName() const
{
    return "StopPortMapServerReply";
}

void StopPortMapServerReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopPortMapServerReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StopPortMapServerReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopPortMapServerReply::~StopPortMapServerReply()
{
}

std::unique_ptr<sngxml::dom::Element> StopPortMapServerReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int PortLease::classId = -1;

PortLease::PortLease()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), portNumber(), processName(), pid(), leaseStartTime(), leaseRenewalTime(), leaseState(), dataPtr(nullptr)
{
}

void* PortLease::Create()
{
    return new PortLease();
}

void PortLease::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<PortLease>(classId, &PortLease::Create);
}

std::string PortLease::StaticClassName()
{
    return "PortLease";
}

std::string PortLease::ClassName() const
{
    return "PortLease";
}

void PortLease::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "portNumber", portNumber);
    sngxml::xmlser::FromXml(element, "processName", processName);
    sngxml::xmlser::FromXml(element, "pid", pid);
    sngxml::xmlser::FromXml(element, "leaseStartTime", leaseStartTime);
    sngxml::xmlser::FromXml(element, "leaseRenewalTime", leaseRenewalTime);
    sngxml::xmlser::FromXml(element, "leaseState", leaseState);
}

std::vector<sngxml::xmlser::XmlPtrBase*> PortLease::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void PortLease::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

PortLease::~PortLease()
{
}

std::unique_ptr<sngxml::dom::Element> PortLease::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portNumber, "portNumber").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(processName, "processName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(pid, "pid").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(leaseStartTime, "leaseStartTime").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(leaseRenewalTime, "leaseRenewalTime").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(leaseState, "leaseState").release()));
    return element;
}

int ViewPortLeaseRequest::classId = -1;

ViewPortLeaseRequest::ViewPortLeaseRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* ViewPortLeaseRequest::Create()
{
    return new ViewPortLeaseRequest();
}

void ViewPortLeaseRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ViewPortLeaseRequest>(classId, &ViewPortLeaseRequest::Create);
}

std::string ViewPortLeaseRequest::StaticClassName()
{
    return "ViewPortLeaseRequest";
}

std::string ViewPortLeaseRequest::ClassName() const
{
    return "ViewPortLeaseRequest";
}

void ViewPortLeaseRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> ViewPortLeaseRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ViewPortLeaseRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ViewPortLeaseRequest::~ViewPortLeaseRequest()
{
}

std::unique_ptr<sngxml::dom::Element> ViewPortLeaseRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int ViewPortLeaseReply::classId = -1;

ViewPortLeaseReply::ViewPortLeaseReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), portLeases(), dataPtr(nullptr)
{
}

void* ViewPortLeaseReply::Create()
{
    return new ViewPortLeaseReply();
}

void ViewPortLeaseReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ViewPortLeaseReply>(classId, &ViewPortLeaseReply::Create);
}

std::string ViewPortLeaseReply::StaticClassName()
{
    return "ViewPortLeaseReply";
}

std::string ViewPortLeaseReply::ClassName() const
{
    return "ViewPortLeaseReply";
}

void ViewPortLeaseReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "portLeases", portLeases);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ViewPortLeaseReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ViewPortLeaseReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ViewPortLeaseReply::~ViewPortLeaseReply()
{
}

std::unique_ptr<sngxml::dom::Element> ViewPortLeaseReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portLeases, "portLeases").release()));
    return element;
}

