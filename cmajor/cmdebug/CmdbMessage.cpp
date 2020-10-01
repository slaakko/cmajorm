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

Location::Location(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "func", func);
    sngjson::json::FromJson(__json_value, "file", file);
    sngjson::json::FromJson(__json_value, "line", line);
    sngjson::json::FromJson(__json_value, "scol", scol);
    sngjson::json::FromJson(__json_value, "ecol", ecol);
    sngjson::json::FromJson(__json_value, "level", level);
}

Type::Type() : name(), id()
{
}

std::unique_ptr<soulng::util::JsonValue> Type::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(name);
    object->AddField(U"name", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(id);
    object->AddField(U"id", std::move(fieldValue1));
    return object;
}

Type::Type(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "name", name);
    sngjson::json::FromJson(__json_value, "id", id);
}

Result::Result() : staticType(), dynamicType(), initialized(), value()
{
}

std::unique_ptr<soulng::util::JsonValue> Result::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(staticType);
    object->AddField(U"staticType", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(dynamicType);
    object->AddField(U"dynamicType", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(initialized);
    object->AddField(U"initialized", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(value);
    object->AddField(U"value", std::move(fieldValue3));
    return object;
}

Result::Result(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "staticType", staticType);
    sngjson::json::FromJson(__json_value, "dynamicType", dynamicType);
    sngjson::json::FromJson(__json_value, "initialized", initialized);
    sngjson::json::FromJson(__json_value, "value", value);
}

ChildResult::ChildResult() : expr(), name(), type(), value(), count()
{
}

std::unique_ptr<soulng::util::JsonValue> ChildResult::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(expr);
    object->AddField(U"expr", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(name);
    object->AddField(U"name", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(type);
    object->AddField(U"type", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(value);
    object->AddField(U"value", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(count);
    object->AddField(U"count", std::move(fieldValue4));
    return object;
}

ChildResult::ChildResult(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "expr", expr);
    sngjson::json::FromJson(__json_value, "name", name);
    sngjson::json::FromJson(__json_value, "type", type);
    sngjson::json::FromJson(__json_value, "value", value);
    sngjson::json::FromJson(__json_value, "count", count);
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

TargetState::TargetState(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "stopReason", stopReason);
    sngjson::json::FromJson(__json_value, "exitCode", exitCode);
    sngjson::json::FromJson(__json_value, "breakpointId", breakpointId);
    sngjson::json::FromJson(__json_value, "signalName", signalName);
    sngjson::json::FromJson(__json_value, "signalMeaning", signalMeaning);
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

SourceLoc::SourceLoc(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "path", path);
    sngjson::json::FromJson(__json_value, "line", line);
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

BreakpointInfo::BreakpointInfo(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "breakpointId", breakpointId);
    sngjson::json::FromJson(__json_value, "numInsts", numInsts);
    sngjson::json::FromJson(__json_value, "location", location);
    sngjson::json::FromJson(__json_value, "error", error);
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

StartRequest::StartRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "breakpointLocations", breakpointLocations);
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

StartReply::StartReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "location", location);
    sngjson::json::FromJson(__json_value, "state", state);
    sngjson::json::FromJson(__json_value, "breakpointInfos", breakpointInfos);
    sngjson::json::FromJson(__json_value, "error", error);
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

GenericErrorReply::GenericErrorReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "errorMessage", errorMessage);
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

ContinueRequest::ContinueRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
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

ContinueReply::ContinueReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "location", location);
    sngjson::json::FromJson(__json_value, "state", state);
    sngjson::json::FromJson(__json_value, "error", error);
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

NextRequest::NextRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
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

NextReply::NextReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "location", location);
    sngjson::json::FromJson(__json_value, "state", state);
    sngjson::json::FromJson(__json_value, "error", error);
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

StepRequest::StepRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
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

StepReply::StepReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "location", location);
    sngjson::json::FromJson(__json_value, "state", state);
    sngjson::json::FromJson(__json_value, "error", error);
}

FinishRequest::FinishRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> FinishRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

FinishRequest::FinishRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

FinishReply::FinishReply() : messageKind(), success(), location(), state(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> FinishReply::ToJson() const
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

FinishReply::FinishReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "location", location);
    sngjson::json::FromJson(__json_value, "state", state);
    sngjson::json::FromJson(__json_value, "error", error);
}

UntilRequest::UntilRequest() : messageKind(), sourceLoc()
{
}

std::unique_ptr<soulng::util::JsonValue> UntilRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(sourceLoc);
    object->AddField(U"sourceLoc", std::move(fieldValue1));
    return object;
}

UntilRequest::UntilRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "sourceLoc", sourceLoc);
}

UntilReply::UntilReply() : messageKind(), success(), location(), state(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> UntilReply::ToJson() const
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

UntilReply::UntilReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "location", location);
    sngjson::json::FromJson(__json_value, "state", state);
    sngjson::json::FromJson(__json_value, "error", error);
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

BreakRequest::BreakRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "breakpointLocation", breakpointLocation);
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

BreakReply::BreakReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "breakpointInfo", breakpointInfo);
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

DeleteRequest::DeleteRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "breakpointId", breakpointId);
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

DeleteReply::DeleteReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "error", error);
}

DepthRequest::DepthRequest() : messageKind()
{
}

std::unique_ptr<soulng::util::JsonValue> DepthRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    return object;
}

DepthRequest::DepthRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

DepthReply::DepthReply() : messageKind(), success(), depth(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> DepthReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(depth);
    object->AddField(U"depth", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

DepthReply::DepthReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "depth", depth);
    sngjson::json::FromJson(__json_value, "error", error);
}

FramesRequest::FramesRequest() : messageKind(), lowFrame(), highFrame()
{
}

std::unique_ptr<soulng::util::JsonValue> FramesRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(lowFrame);
    object->AddField(U"lowFrame", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(highFrame);
    object->AddField(U"highFrame", std::move(fieldValue2));
    return object;
}

FramesRequest::FramesRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "lowFrame", lowFrame);
    sngjson::json::FromJson(__json_value, "highFrame", highFrame);
}

FramesReply::FramesReply() : messageKind(), success(), frames(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> FramesReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(frames);
    object->AddField(U"frames", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

FramesReply::FramesReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "frames", frames);
    sngjson::json::FromJson(__json_value, "error", error);
}

EvaluateRequest::EvaluateRequest() : messageKind(), expression()
{
}

std::unique_ptr<soulng::util::JsonValue> EvaluateRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(expression);
    object->AddField(U"expression", std::move(fieldValue1));
    return object;
}

EvaluateRequest::EvaluateRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "expression", expression);
}

EvaluateReply::EvaluateReply() : messageKind(), success(), result(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> EvaluateReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(result);
    object->AddField(U"result", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

EvaluateReply::EvaluateReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "result", result);
    sngjson::json::FromJson(__json_value, "error", error);
}

CountRequest::CountRequest() : messageKind(), expression()
{
}

std::unique_ptr<soulng::util::JsonValue> CountRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(expression);
    object->AddField(U"expression", std::move(fieldValue1));
    return object;
}

CountRequest::CountRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "expression", expression);
}

CountReply::CountReply() : messageKind(), success(), count(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> CountReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(count);
    object->AddField(U"count", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

CountReply::CountReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "count", count);
    sngjson::json::FromJson(__json_value, "error", error);
}

EvaluateChildRequest::EvaluateChildRequest() : messageKind(), expression(), start(), count()
{
}

std::unique_ptr<soulng::util::JsonValue> EvaluateChildRequest::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(expression);
    object->AddField(U"expression", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(start);
    object->AddField(U"start", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(count);
    object->AddField(U"count", std::move(fieldValue3));
    return object;
}

EvaluateChildRequest::EvaluateChildRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "expression", expression);
    sngjson::json::FromJson(__json_value, "start", start);
    sngjson::json::FromJson(__json_value, "count", count);
}

EvaluateChildReply::EvaluateChildReply() : messageKind(), success(), results(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> EvaluateChildReply::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(messageKind);
    object->AddField(U"messageKind", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(success);
    object->AddField(U"success", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(results);
    object->AddField(U"results", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

EvaluateChildReply::EvaluateChildReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "success", success);
    sngjson::json::FromJson(__json_value, "results", results);
    sngjson::json::FromJson(__json_value, "error", error);
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

TargetRunningRequest::TargetRunningRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
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

TargetRunningReply::TargetRunningReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
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

TargetInputRequest::TargetInputRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
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

TargetInputReply::TargetInputReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "line", line);
    sngjson::json::FromJson(__json_value, "eof", eof);
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

TargetOutputRequest::TargetOutputRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "handle", handle);
    sngjson::json::FromJson(__json_value, "output", output);
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

TargetOutputReply::TargetOutputReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
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

LogMessageRequest::LogMessageRequest(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
    sngjson::json::FromJson(__json_value, "logMessage", logMessage);
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

LogMessageReply::LogMessageReply(soulng::util::JsonValue* __json_value)
{
    sngjson::json::FromJson(__json_value, "messageKind", messageKind);
}

