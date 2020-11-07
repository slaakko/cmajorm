#ifndef HPP_E9A7CDC085758979427A3E510D8D38DC3640E725
#define HPP_E9A7CDC085758979427A3E510D8D38DC3640E725
#include <soulng/util/Json.hpp>
#include <string>
#include <vector>
#include <memory>

struct GetFreePortNumberRequest
{
    GetFreePortNumberRequest();
    GetFreePortNumberRequest(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string message;
    std::string programName;
    std::string pid;
    std::string time;
};

struct GetFreePortNumberReply
{
    GetFreePortNumberReply();
    GetFreePortNumberReply(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string message;
    std::string portNumber;
};

struct ExtendPortLeaseRequest
{
    ExtendPortLeaseRequest();
    ExtendPortLeaseRequest(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string message;
    std::string programName;
    std::string pid;
    std::vector<std::string> portNumbers;
};

struct ExtendPortLeaseReply
{
    ExtendPortLeaseReply();
    ExtendPortLeaseReply(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string message;
};

struct StopPortNumberServerRequest
{
    StopPortNumberServerRequest();
    StopPortNumberServerRequest(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string message;
};

#endif // HPP_E9A7CDC085758979427A3E510D8D38DC3640E725