#include "CmdbMessage.hpp"
#include <sngjson/json/JsonImport.hpp>
#include <sngjson/json/JsonExport.hpp>
#include <utility>

Location::Location() : func(), file(), line(), scol(), ecol(), level()
{
}

std::unique_ptr<soulng::util::JsonValue> Location::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(func);
    object->AddField(U"func", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(file);
    object->AddField(U"file", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(line);
    object->AddField(U"line", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(scol);
    object->AddField(U"scol", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(ecol);
    object->AddField(U"ecol", std::move(fieldValue4));
    std::unique_ptr<soulng::util::JsonValue> fieldValue5 = sngjson::json::ToJson(level);
    object->AddField(U"level", std::move(fieldValue5));
    return object;
}

Location::Location(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "func", func);
    sngjson::json::FromJson(value, "file", file);
    sngjson::json::FromJson(value, "line", line);
    sngjson::json::FromJson(value, "scol", scol);
    sngjson::json::FromJson(value, "ecol", ecol);
    sngjson::json::FromJson(value, "level", level);
}

TargetState::TargetState() : stopReason(), exitCode(), breakpointId(), signalName(), signalMeaning()
{
}

std::unique_ptr<soulng::util::JsonValue> TargetState::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(stopReason);
    object->AddField(U"stopReason", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(exitCode);
    object->AddField(U"exitCode", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(breakpointId);
    object->AddField(U"breakpointId", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(signalName);
    object->AddField(U"signalName", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(signalMeaning);
    object->AddField(U"signalMeaning", std::move(fieldValue4));
    return object;
}

TargetState::TargetState(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "stopReason", stopReason);
    sngjson::json::FromJson(value, "exitCode", exitCode);
    sngjson::json::FromJson(value, "breakpointId", breakpointId);
    sngjson::json::FromJson(value, "signalName", signalName);
    sngjson::json::FromJson(value, "signalMeaning", signalMeaning);
}

SourceLoc::SourceLoc() : path(), line()
{
}

std::unique_ptr<soulng::util::JsonValue> SourceLoc::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(path);
    object->AddField(U"path", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(line);
    object->AddField(U"line", std::move(fieldValue1));
    return object;
}

SourceLoc::SourceLoc(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "path", path);
    sngjson::json::FromJson(value, "line", line);
}

BreakpointInfo::BreakpointInfo() : success(), breakpointId(), numInsts(), location(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> BreakpointInfo::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(breakpointId);
    object->AddField(U"breakpointId", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(numInsts);
    object->AddField(U"numInsts", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(location);
    object->AddField(U"location", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue4));
    return object;
}

BreakpointInfo::BreakpointInfo(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "success", success);
    sngjson::json::FromJson(value, "breakpointId", breakpointId);
    sngjson::json::FromJson(value, "numInsts", numInsts);
    sngjson::json::FromJson(value, "location", location);
    sngjson::json::FromJson(value, "error", error);
}

StartRequest::StartRequest() : messageKind(), breakpointLocations()
{
}

std::unique_ptr<soulng::util::JsonValue> StartRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(breakpointLocations);
    object->AddField(U"breakpointLocations", std::move(fieldValue1));
    return object;
}

StartRequest::StartRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "breakpointLocations", breakpointLocations);
}

StartReply::StartReply() : messageKind(), success(), location(), state(), breakpointInfos(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> StartReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(location);
    object->AddField(U"location", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(state);
    object->AddField(U"state", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(breakpointInfos);
    object->AddField(U"breakpointInfos", std::move(fieldValue4));
    std::unique_ptr<soulng::util::JsonValue> fieldValue5 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue5));
    return object;
}

StartReply::StartReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "success", success);
    sngjson::json::FromJson(value, "location", location);
    sngjson::json::FromJson(value, "state", state);
    sngjson::json::FromJson(value, "breakpointInfos", breakpointInfos);
    sngjson::json::FromJson(value, "error", error);
}

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

StopRequest::StopRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
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

StopReply::StopReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

GenericErrorReply::GenericErrorReply() : messageKind(), errorMessage()
{
}

std::unique_ptr<soulng::util::JsonValue> GenericErrorReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(errorMessage);
    object->AddField(U"errorMessage", std::move(fieldValue1));
    return object;
}

GenericErrorReply::GenericErrorReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "errorMessage", errorMessage);
}

ContinueRequest::ContinueRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> ContinueRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

ContinueRequest::ContinueRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

ContinueReply::ContinueReply() : messageKind(), success(), location(), state(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> ContinueReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(location);
    object->AddField(U"location", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(state);
    object->AddField(U"state", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue4));
    return object;
}

ContinueReply::ContinueReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "success", success);
    sngjson::json::FromJson(value, "location", location);
    sngjson::json::FromJson(value, "state", state);
    sngjson::json::FromJson(value, "error", error);
}

NextRequest::NextRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> NextRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

NextRequest::NextRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

NextReply::NextReply() : messageKind(), success(), location(), state(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> NextReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(location);
    object->AddField(U"location", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(state);
    object->AddField(U"state", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue4));
    return object;
}

NextReply::NextReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "success", success);
    sngjson::json::FromJson(value, "location", location);
    sngjson::json::FromJson(value, "state", state);
    sngjson::json::FromJson(value, "error", error);
}

StepRequest::StepRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> StepRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

StepRequest::StepRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

StepReply::StepReply() : messageKind(), success(), location(), state(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> StepReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(location);
    object->AddField(U"location", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(state);
    object->AddField(U"state", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue4));
    return object;
}

StepReply::StepReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "success", success);
    sngjson::json::FromJson(value, "location", location);
    sngjson::json::FromJson(value, "state", state);
    sngjson::json::FromJson(value, "error", error);
}

BreakRequest::BreakRequest() : messageKind(), breakpointLocation()
{
}

std::unique_ptr<soulng::util::JsonValue> BreakRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(breakpointLocation);
    object->AddField(U"breakpointLocation", std::move(fieldValue1));
    return object;
}

BreakRequest::BreakRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "breakpointLocation", breakpointLocation);
}

BreakReply::BreakReply() : messageKind(), breakpointInfo()
{
}

std::unique_ptr<soulng::util::JsonValue> BreakReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(breakpointInfo);
    object->AddField(U"breakpointInfo", std::move(fieldValue1));
    return object;
}

BreakReply::BreakReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "breakpointInfo", breakpointInfo);
}

DeleteRequest::DeleteRequest() : messageKind(), breakpointId()
{
}

std::unique_ptr<soulng::util::JsonValue> DeleteRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(breakpointId);
    object->AddField(U"breakpointId", std::move(fieldValue1));
    return object;
}

DeleteRequest::DeleteRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "breakpointId", breakpointId);
}

DeleteReply::DeleteReply() : messageKind(), success(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> DeleteReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue2));
    return object;
}

DeleteReply::DeleteReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "success", success);
    sngjson::json::FromJson(value, "error", error);
}

TargetRunningRequest::TargetRunningRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> TargetRunningRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

TargetRunningRequest::TargetRunningRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

TargetRunningReply::TargetRunningReply() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> TargetRunningReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

TargetRunningReply::TargetRunningReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

TargetInputRequest::TargetInputRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> TargetInputRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

TargetInputRequest::TargetInputRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

TargetInputReply::TargetInputReply() : messageKind(), line(), eof()
{
}

std::unique_ptr<soulng::util::JsonValue> TargetInputReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(line);
    object->AddField(U"line", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(eof);
    object->AddField(U"eof", std::move(fieldValue2));
    return object;
}

TargetInputReply::TargetInputReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "line", line);
    sngjson::json::FromJson(value, "eof", eof);
}

TargetOutputRequest::TargetOutputRequest() : messageKind(), handle(), output()
{
}

std::unique_ptr<soulng::util::JsonValue> TargetOutputRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(handle);
    object->AddField(U"handle", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(output);
    object->AddField(U"output", std::move(fieldValue2));
    return object;
}

TargetOutputRequest::TargetOutputRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "handle", handle);
    sngjson::json::FromJson(value, "output", output);
}

TargetOutputReply::TargetOutputReply() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> TargetOutputReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

TargetOutputReply::TargetOutputReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

LogMessageRequest::LogMessageRequest() : messageKind(), logMessage()
{
}

std::unique_ptr<soulng::util::JsonValue> LogMessageRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(logMessage);
    object->AddField(U"logMessage", std::move(fieldValue1));
    return object;
}

LogMessageRequest::LogMessageRequest(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
    sngjson::json::FromJson(value, "logMessage", logMessage);
}

LogMessageReply::LogMessageReply() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> LogMessageReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

LogMessageReply::LogMessageReply(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "messageKind", messageKind);
}

