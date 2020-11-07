#include "PortMapMessage.hpp"
#include <sngjson/json/JsonImport.hpp>
#include <sngjson/json/JsonExport.hpp>
#include <utility>

GetFreePortNumberRequest::GetFreePortNumberRequest() : message(), programName(), pid(), time()
{
}

std::unique_ptr<soulng::util::JsonValue> GetFreePortNumberRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(message);
    object->AddField(U"message", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(programName);
    object->AddField(U"programName", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(pid);
    object->AddField(U"pid", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(time);
    object->AddField(U"time", std::move(fieldValue3));
    return object;
}

GetFreePortNumberRequest::GetFreePortNumberRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "message", message);
    sngjson::json::FromJson(__json_value, "programName", programName);
    sngjson::json::FromJson(__json_value, "pid", pid);
    sngjson::json::FromJson(__json_value, "time", time);
}

GetFreePortNumberReply::GetFreePortNumberReply() : message(), portNumber()
{
}

std::unique_ptr<soulng::util::JsonValue> GetFreePortNumberReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(message);
    object->AddField(U"message", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(portNumber);
    object->AddField(U"portNumber", std::move(fieldValue1));
    return object;
}

GetFreePortNumberReply::GetFreePortNumberReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "message", message);
    sngjson::json::FromJson(__json_value, "portNumber", portNumber);
}

ExtendPortLeaseRequest::ExtendPortLeaseRequest() : message(), programName(), pid(), portNumbers()
{
}

std::unique_ptr<soulng::util::JsonValue> ExtendPortLeaseRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(message);
    object->AddField(U"message", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(programName);
    object->AddField(U"programName", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(pid);
    object->AddField(U"pid", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(portNumbers);
    object->AddField(U"portNumbers", std::move(fieldValue3));
    return object;
}

ExtendPortLeaseRequest::ExtendPortLeaseRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "message", message);
    sngjson::json::FromJson(__json_value, "programName", programName);
    sngjson::json::FromJson(__json_value, "pid", pid);
    sngjson::json::FromJson(__json_value, "portNumbers", portNumbers);
}

ExtendPortLeaseReply::ExtendPortLeaseReply() : message()
{
}

std::unique_ptr<soulng::util::JsonValue> ExtendPortLeaseReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(message);
    object->AddField(U"message", std::move(fieldValue0));
    return object;
}

ExtendPortLeaseReply::ExtendPortLeaseReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "message", message);
}

StopPortNumberServerRequest::StopPortNumberServerRequest() : message()
{
}

std::unique_ptr<soulng::util::JsonValue> StopPortNumberServerRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(message);
    object->AddField(U"message", std::move(fieldValue0));
    return object;
}

StopPortNumberServerRequest::StopPortNumberServerRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "message", message);
}

