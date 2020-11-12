#include "BuildServerMessage.hpp"
#include <sngjson/json/JsonImport.hpp>
#include <sngjson/json/JsonExport.hpp>
#include <utility>

StopRequest::StopRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> StopRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

StopRequest::StopRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

StopReply::StopReply() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> StopReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

StopReply::StopReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

KeepAliveRequest::KeepAliveRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> KeepAliveRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

KeepAliveRequest::KeepAliveRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

KeepAliveReply::KeepAliveReply() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> KeepAliveReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

KeepAliveReply::KeepAliveReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

BuildRequest::BuildRequest() : messageKind(), backend(), config(), optimizationLevel(), filePath(), defines(), numBuildThreads(), toolChain(), verbose(), quiet(), clean(), strictNothrow(), emitIR(), emitOptIR(), linkWithDebugRuntime(), linkUsingMsLink(), rebuild(), buildAllDependencies(), forceDebugInfo(), forceNoDebugInfo(), disableModuleCache(), singleThreadedCompile(), disableCodeGen(), justMyCode()
{
}

std::unique_ptr<soulng::util::JsonValue> BuildRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(backend);
    object->AddField(U"backend", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(config);
    object->AddField(U"config", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(optimizationLevel);
    object->AddField(U"optimizationLevel", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(filePath);
    object->AddField(U"filePath", std::move(fieldValue4));
    std::unique_ptr<soulng::util::JsonValue> fieldValue5 = sngjson::json::ToJson(defines);
    object->AddField(U"defines", std::move(fieldValue5));
    std::unique_ptr<soulng::util::JsonValue> fieldValue6 = sngjson::json::ToJson(numBuildThreads);
    object->AddField(U"numBuildThreads", std::move(fieldValue6));
    std::unique_ptr<soulng::util::JsonValue> fieldValue7 = sngjson::json::ToJson(toolChain);
    object->AddField(U"toolChain", std::move(fieldValue7));
    std::unique_ptr<soulng::util::JsonValue> fieldValue8 = sngjson::json::ToJson(verbose);
    object->AddField(U"verbose", std::move(fieldValue8));
    std::unique_ptr<soulng::util::JsonValue> fieldValue9 = sngjson::json::ToJson(quiet);
    object->AddField(U"quiet", std::move(fieldValue9));
    std::unique_ptr<soulng::util::JsonValue> fieldValue10 = sngjson::json::ToJson(clean);
    object->AddField(U"clean", std::move(fieldValue10));
    std::unique_ptr<soulng::util::JsonValue> fieldValue11 = sngjson::json::ToJson(strictNothrow);
    object->AddField(U"strictNothrow", std::move(fieldValue11));
    std::unique_ptr<soulng::util::JsonValue> fieldValue12 = sngjson::json::ToJson(emitIR);
    object->AddField(U"emitIR", std::move(fieldValue12));
    std::unique_ptr<soulng::util::JsonValue> fieldValue13 = sngjson::json::ToJson(emitOptIR);
    object->AddField(U"emitOptIR", std::move(fieldValue13));
    std::unique_ptr<soulng::util::JsonValue> fieldValue14 = sngjson::json::ToJson(linkWithDebugRuntime);
    object->AddField(U"linkWithDebugRuntime", std::move(fieldValue14));
    std::unique_ptr<soulng::util::JsonValue> fieldValue15 = sngjson::json::ToJson(linkUsingMsLink);
    object->AddField(U"linkUsingMsLink", std::move(fieldValue15));
    std::unique_ptr<soulng::util::JsonValue> fieldValue16 = sngjson::json::ToJson(rebuild);
    object->AddField(U"rebuild", std::move(fieldValue16));
    std::unique_ptr<soulng::util::JsonValue> fieldValue17 = sngjson::json::ToJson(buildAllDependencies);
    object->AddField(U"buildAllDependencies", std::move(fieldValue17));
    std::unique_ptr<soulng::util::JsonValue> fieldValue18 = sngjson::json::ToJson(forceDebugInfo);
    object->AddField(U"forceDebugInfo", std::move(fieldValue18));
    std::unique_ptr<soulng::util::JsonValue> fieldValue19 = sngjson::json::ToJson(forceNoDebugInfo);
    object->AddField(U"forceNoDebugInfo", std::move(fieldValue19));
    std::unique_ptr<soulng::util::JsonValue> fieldValue20 = sngjson::json::ToJson(disableModuleCache);
    object->AddField(U"disableModuleCache", std::move(fieldValue20));
    std::unique_ptr<soulng::util::JsonValue> fieldValue21 = sngjson::json::ToJson(singleThreadedCompile);
    object->AddField(U"singleThreadedCompile", std::move(fieldValue21));
    std::unique_ptr<soulng::util::JsonValue> fieldValue22 = sngjson::json::ToJson(disableCodeGen);
    object->AddField(U"disableCodeGen", std::move(fieldValue22));
    std::unique_ptr<soulng::util::JsonValue> fieldValue23 = sngjson::json::ToJson(justMyCode);
    object->AddField(U"justMyCode", std::move(fieldValue23));
    return object;
}

BuildRequest::BuildRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "backend", backend);
    sngjson::json::FromJson(__json_value, "config", config);
    sngjson::json::FromJson(__json_value, "optimizationLevel", optimizationLevel);
    sngjson::json::FromJson(__json_value, "filePath", filePath);
    sngjson::json::FromJson(__json_value, "defines", defines);
    sngjson::json::FromJson(__json_value, "numBuildThreads", numBuildThreads);
    sngjson::json::FromJson(__json_value, "toolChain", toolChain);
    sngjson::json::FromJson(__json_value, "verbose", verbose);
    sngjson::json::FromJson(__json_value, "quiet", quiet);
    sngjson::json::FromJson(__json_value, "clean", clean);
    sngjson::json::FromJson(__json_value, "strictNothrow", strictNothrow);
    sngjson::json::FromJson(__json_value, "emitIR", emitIR);
    sngjson::json::FromJson(__json_value, "emitOptIR", emitOptIR);
    sngjson::json::FromJson(__json_value, "linkWithDebugRuntime", linkWithDebugRuntime);
    sngjson::json::FromJson(__json_value, "linkUsingMsLink", linkUsingMsLink);
    sngjson::json::FromJson(__json_value, "rebuild", rebuild);
    sngjson::json::FromJson(__json_value, "buildAllDependencies", buildAllDependencies);
    sngjson::json::FromJson(__json_value, "forceDebugInfo", forceDebugInfo);
    sngjson::json::FromJson(__json_value, "forceNoDebugInfo", forceNoDebugInfo);
    sngjson::json::FromJson(__json_value, "disableModuleCache", disableModuleCache);
    sngjson::json::FromJson(__json_value, "singleThreadedCompile", singleThreadedCompile);
    sngjson::json::FromJson(__json_value, "disableCodeGen", disableCodeGen);
    sngjson::json::FromJson(__json_value, "justMyCode", justMyCode);
}

BuildReply::BuildReply() : messageKind(), requestValid(), requestErrorMessage(), logException(), time(), success(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> BuildReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(requestValid);
    object->AddField(U"requestValid", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(requestErrorMessage);
    object->AddField(U"requestErrorMessage", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(logException);
    object->AddField(U"logException", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(time);
    object->AddField(U"time", std::move(fieldValue4));
    std::unique_ptr<soulng::util::JsonValue> fieldValue5 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue5));
    std::unique_ptr<soulng::util::JsonValue> fieldValue6 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue6));
    return object;
}

BuildReply::BuildReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "requestValid", requestValid);
    sngjson::json::FromJson(__json_value, "requestErrorMessage", requestErrorMessage);
    sngjson::json::FromJson(__json_value, "logException", logException);
    sngjson::json::FromJson(__json_value, "time", time);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "error", error);
}

LogMessageRequest::LogMessageRequest() : messageKind(), message()
{
}

std::unique_ptr<soulng::util::JsonValue> LogMessageRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(message);
    object->AddField(U"message", std::move(fieldValue1));
    return object;
}

LogMessageRequest::LogMessageRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "message", message);
}

LogMessageReply::LogMessageReply() : messageKind(), ok()
{
}

std::unique_ptr<soulng::util::JsonValue> LogMessageReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(ok);
    object->AddField(U"ok", std::move(fieldValue1));
    return object;
}

LogMessageReply::LogMessageReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "ok", ok);
}

ProgressMessage::ProgressMessage() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> ProgressMessage::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

ProgressMessage::ProgressMessage(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

GenericErrorReply::GenericErrorReply() : messageKind(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> GenericErrorReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue1));
    return object;
}

GenericErrorReply::GenericErrorReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "error", error);
}

