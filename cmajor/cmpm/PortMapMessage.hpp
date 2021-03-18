#ifndef PORTMAPMESSAGE_HPP_DC8E9733F60B35A671F1CC8A29699D85F1D0FE2B
#define PORTMAPMESSAGE_HPP_DC8E9733F60B35A671F1CC8A29699D85F1D0FE2B
#include <cmajor/cmpm/Api.hpp>
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

class CMPM_API GetFreePortNumberRequest
{
public:
    GetFreePortNumberRequest();
    GetFreePortNumberRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string processName;
    int32_t pid;
};

class CMPM_API GetFreePortNumberReply
{
public:
    GetFreePortNumberReply();
    GetFreePortNumberReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    int32_t portNumber;
};

class CMPM_API ExtendPortLeaseRequest
{
public:
    ExtendPortLeaseRequest();
    ExtendPortLeaseRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string processName;
    int32_t pid;
    std::vector<int32_t> portNumbers;
};

class CMPM_API ExtendPortLeaseReply
{
public:
    ExtendPortLeaseReply();
    ExtendPortLeaseReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMPM_API StopPortMapServerRequest
{
public:
    StopPortMapServerRequest();
    StopPortMapServerRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMPM_API StopPortMapServerReply
{
public:
    StopPortMapServerReply();
    StopPortMapServerReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMPM_API PortLease
{
public:
    PortLease();
    PortLease(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    int32_t portNumber;
    std::string processName;
    int32_t pid;
    datetime leaseStartTime;
    datetime leaseRenewalTime;
    std::string leaseState;
};

class CMPM_API ViewPortLeaseRequest
{
public:
    ViewPortLeaseRequest();
    ViewPortLeaseRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMPM_API ViewPortLeaseReply
{
public:
    ViewPortLeaseReply();
    ViewPortLeaseReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::vector<PortLease> portLeases;
};

#endif // PORTMAPMESSAGE_HPP_DC8E9733F60B35A671F1CC8A29699D85F1D0FE2B
