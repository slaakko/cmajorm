#include "PortMapMessage.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

HelloPmsRequest::HelloPmsRequest()
{
}

HelloPmsRequest::HelloPmsRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> HelloPmsRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

HelloPmsReply::HelloPmsReply()
    : version()
{
}

HelloPmsReply::HelloPmsReply(sngxml::dom::Element* element)
    : version()
{
    sngxml::xmlser::FromXml(element, "version", version);
}

std::unique_ptr<sngxml::dom::Element> HelloPmsReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(version, "version").release()));
    return element;
}

GetFreePortNumberRequest::GetFreePortNumberRequest()
    : processName(), pid()
{
}

GetFreePortNumberRequest::GetFreePortNumberRequest(sngxml::dom::Element* element)
    : processName(), pid()
{
    sngxml::xmlser::FromXml(element, "processName", processName);
    sngxml::xmlser::FromXml(element, "pid", pid);
}

std::unique_ptr<sngxml::dom::Element> GetFreePortNumberRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(processName, "processName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(pid, "pid").release()));
    return element;
}

GetFreePortNumberReply::GetFreePortNumberReply()
    : portNumber()
{
}

GetFreePortNumberReply::GetFreePortNumberReply(sngxml::dom::Element* element)
    : portNumber()
{
    sngxml::xmlser::FromXml(element, "portNumber", portNumber);
}

std::unique_ptr<sngxml::dom::Element> GetFreePortNumberReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portNumber, "portNumber").release()));
    return element;
}

ExtendPortLeaseRequest::ExtendPortLeaseRequest()
    : processName(), pid(), portNumbers()
{
}

ExtendPortLeaseRequest::ExtendPortLeaseRequest(sngxml::dom::Element* element)
    : processName(), pid(), portNumbers()
{
    sngxml::xmlser::FromXml(element, "processName", processName);
    sngxml::xmlser::FromXml(element, "pid", pid);
    sngxml::xmlser::FromXml(element, "portNumbers", portNumbers);
}

std::unique_ptr<sngxml::dom::Element> ExtendPortLeaseRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(processName, "processName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(pid, "pid").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portNumbers, "portNumbers").release()));
    return element;
}

ExtendPortLeaseReply::ExtendPortLeaseReply()
{
}

ExtendPortLeaseReply::ExtendPortLeaseReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> ExtendPortLeaseReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

StopPortMapServerRequest::StopPortMapServerRequest()
{
}

StopPortMapServerRequest::StopPortMapServerRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> StopPortMapServerRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

StopPortMapServerReply::StopPortMapServerReply()
{
}

StopPortMapServerReply::StopPortMapServerReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> StopPortMapServerReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

PortLease::PortLease()
    : portNumber(), processName(), pid(), leaseStartTime(), leaseRenewalTime(), leaseState()
{
}

PortLease::PortLease(sngxml::dom::Element* element)
    : portNumber(), processName(), pid(), leaseStartTime(), leaseRenewalTime(), leaseState()
{
    sngxml::xmlser::FromXml(element, "portNumber", portNumber);
    sngxml::xmlser::FromXml(element, "processName", processName);
    sngxml::xmlser::FromXml(element, "pid", pid);
    sngxml::xmlser::FromXml(element, "leaseStartTime", leaseStartTime);
    sngxml::xmlser::FromXml(element, "leaseRenewalTime", leaseRenewalTime);
    sngxml::xmlser::FromXml(element, "leaseState", leaseState);
}

std::unique_ptr<sngxml::dom::Element> PortLease::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portNumber, "portNumber").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(processName, "processName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(pid, "pid").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(leaseStartTime, "leaseStartTime").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(leaseRenewalTime, "leaseRenewalTime").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(leaseState, "leaseState").release()));
    return element;
}

ViewPortLeaseRequest::ViewPortLeaseRequest()
{
}

ViewPortLeaseRequest::ViewPortLeaseRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> ViewPortLeaseRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

ViewPortLeaseReply::ViewPortLeaseReply()
    : portLeases()
{
}

ViewPortLeaseReply::ViewPortLeaseReply(sngxml::dom::Element* element)
    : portLeases()
{
    sngxml::xmlser::FromXml(element, "portLeases", portLeases);
}

std::unique_ptr<sngxml::dom::Element> ViewPortLeaseReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(portLeases, "portLeases").release()));
    return element;
}

