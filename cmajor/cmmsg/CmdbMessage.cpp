#include "CmdbMessage.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

Location::Location()
    : func(), file(), line(), scol(), ecol(), level()
{
}

Location::Location(sngxml::dom::Element* element)
    : func(), file(), line(), scol(), ecol(), level()
{
    sngxml::xmlser::FromXml(element, "func", func);
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "scol", scol);
    sngxml::xmlser::FromXml(element, "ecol", ecol);
    sngxml::xmlser::FromXml(element, "level", level);
}

std::unique_ptr<sngxml::dom::Element> Location::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(func, "func").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(scol, "scol").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ecol, "ecol").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(level, "level").release()));
    return element;
}

Type::Type()
    : name(), id()
{
}

Type::Type(sngxml::dom::Element* element)
    : name(), id()
{
    sngxml::xmlser::FromXml(element, "name", name);
    sngxml::xmlser::FromXml(element, "id", id);
}

std::unique_ptr<sngxml::dom::Element> Type::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(name, "name").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(id, "id").release()));
    return element;
}

Result::Result()
    : staticType(), dynamicType(), initialized(), value()
{
}

Result::Result(sngxml::dom::Element* element)
    : staticType(), dynamicType(), initialized(), value()
{
    sngxml::xmlser::FromXml(element, "staticType", staticType);
    sngxml::xmlser::FromXml(element, "dynamicType", dynamicType);
    sngxml::xmlser::FromXml(element, "initialized", initialized);
    sngxml::xmlser::FromXml(element, "value", value);
}

std::unique_ptr<sngxml::dom::Element> Result::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(staticType, "staticType").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(dynamicType, "dynamicType").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(initialized, "initialized").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(value, "value").release()));
    return element;
}

ChildResult::ChildResult()
    : expr(), name(), type(), dynType(), value(), count()
{
}

ChildResult::ChildResult(sngxml::dom::Element* element)
    : expr(), name(), type(), dynType(), value(), count()
{
    sngxml::xmlser::FromXml(element, "expr", expr);
    sngxml::xmlser::FromXml(element, "name", name);
    sngxml::xmlser::FromXml(element, "type", type);
    sngxml::xmlser::FromXml(element, "dynType", dynType);
    sngxml::xmlser::FromXml(element, "value", value);
    sngxml::xmlser::FromXml(element, "count", count);
}

std::unique_ptr<sngxml::dom::Element> ChildResult::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expr, "expr").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(name, "name").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(type, "type").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(dynType, "dynType").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(value, "value").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(count, "count").release()));
    return element;
}

TargetState::TargetState()
    : stopReason(), exitCode(), breakpointId(), signalName(), signalMeaning()
{
}

TargetState::TargetState(sngxml::dom::Element* element)
    : stopReason(), exitCode(), breakpointId(), signalName(), signalMeaning()
{
    sngxml::xmlser::FromXml(element, "stopReason", stopReason);
    sngxml::xmlser::FromXml(element, "exitCode", exitCode);
    sngxml::xmlser::FromXml(element, "breakpointId", breakpointId);
    sngxml::xmlser::FromXml(element, "signalName", signalName);
    sngxml::xmlser::FromXml(element, "signalMeaning", signalMeaning);
}

std::unique_ptr<sngxml::dom::Element> TargetState::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(stopReason, "stopReason").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(exitCode, "exitCode").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointId, "breakpointId").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(signalName, "signalName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(signalMeaning, "signalMeaning").release()));
    return element;
}

SourceLoc::SourceLoc()
    : path(), line()
{
}

SourceLoc::SourceLoc(sngxml::dom::Element* element)
    : path(), line()
{
    sngxml::xmlser::FromXml(element, "path", path);
    sngxml::xmlser::FromXml(element, "line", line);
}

std::unique_ptr<sngxml::dom::Element> SourceLoc::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(path, "path").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    return element;
}

BreakpointInfo::BreakpointInfo()
    : success(), breakpointId(), numInsts(), location(), error()
{
}

BreakpointInfo::BreakpointInfo(sngxml::dom::Element* element)
    : success(), breakpointId(), numInsts(), location(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "breakpointId", breakpointId);
    sngxml::xmlser::FromXml(element, "numInsts", numInsts);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> BreakpointInfo::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointId, "breakpointId").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(numInsts, "numInsts").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

StartDebugRequest::StartDebugRequest()
    : breakpointLocations()
{
}

StartDebugRequest::StartDebugRequest(sngxml::dom::Element* element)
    : breakpointLocations()
{
    sngxml::xmlser::FromXml(element, "breakpointLocations", breakpointLocations);
}

std::unique_ptr<sngxml::dom::Element> StartDebugRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointLocations, "breakpointLocations").release()));
    return element;
}

StartDebugReply::StartDebugReply()
    : success(), location(), state(), breakpointInfos(), error()
{
}

StartDebugReply::StartDebugReply(sngxml::dom::Element* element)
    : success(), location(), state(), breakpointInfos(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "breakpointInfos", breakpointInfos);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> StartDebugReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointInfos, "breakpointInfos").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

StopDebugRequest::StopDebugRequest()
{
}

StopDebugRequest::StopDebugRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> StopDebugRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

StopDebugReply::StopDebugReply()
{
}

StopDebugReply::StopDebugReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> StopDebugReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

GenericDebugErrorReply::GenericDebugErrorReply()
    : errorMessage()
{
}

GenericDebugErrorReply::GenericDebugErrorReply(sngxml::dom::Element* element)
    : errorMessage()
{
    sngxml::xmlser::FromXml(element, "errorMessage", errorMessage);
}

std::unique_ptr<sngxml::dom::Element> GenericDebugErrorReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(errorMessage, "errorMessage").release()));
    return element;
}

ContinueRequest::ContinueRequest()
{
}

ContinueRequest::ContinueRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> ContinueRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

ContinueReply::ContinueReply()
    : success(), location(), state(), error()
{
}

ContinueReply::ContinueReply(sngxml::dom::Element* element)
    : success(), location(), state(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> ContinueReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

NextRequest::NextRequest()
{
}

NextRequest::NextRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> NextRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

NextReply::NextReply()
    : success(), location(), state(), error()
{
}

NextReply::NextReply(sngxml::dom::Element* element)
    : success(), location(), state(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> NextReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

StepRequest::StepRequest()
{
}

StepRequest::StepRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> StepRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

StepReply::StepReply()
    : success(), location(), state(), error()
{
}

StepReply::StepReply(sngxml::dom::Element* element)
    : success(), location(), state(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> StepReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

FinishRequest::FinishRequest()
{
}

FinishRequest::FinishRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> FinishRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

FinishReply::FinishReply()
    : success(), location(), state(), error()
{
}

FinishReply::FinishReply(sngxml::dom::Element* element)
    : success(), location(), state(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> FinishReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

UntilRequest::UntilRequest()
    : sourceLoc()
{
}

UntilRequest::UntilRequest(sngxml::dom::Element* element)
    : sourceLoc()
{
    sngxml::xmlser::FromXml(element, "sourceLoc", sourceLoc);
}

std::unique_ptr<sngxml::dom::Element> UntilRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceLoc, "sourceLoc").release()));
    return element;
}

UntilReply::UntilReply()
    : success(), location(), state(), error()
{
}

UntilReply::UntilReply(sngxml::dom::Element* element)
    : success(), location(), state(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> UntilReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

BreakRequest::BreakRequest()
    : breakpointLocation()
{
}

BreakRequest::BreakRequest(sngxml::dom::Element* element)
    : breakpointLocation()
{
    sngxml::xmlser::FromXml(element, "breakpointLocation", breakpointLocation);
}

std::unique_ptr<sngxml::dom::Element> BreakRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointLocation, "breakpointLocation").release()));
    return element;
}

BreakReply::BreakReply()
    : breakpointInfo()
{
}

BreakReply::BreakReply(sngxml::dom::Element* element)
    : breakpointInfo()
{
    sngxml::xmlser::FromXml(element, "breakpointInfo", breakpointInfo);
}

std::unique_ptr<sngxml::dom::Element> BreakReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointInfo, "breakpointInfo").release()));
    return element;
}

DeleteRequest::DeleteRequest()
    : breakpointId()
{
}

DeleteRequest::DeleteRequest(sngxml::dom::Element* element)
    : breakpointId()
{
    sngxml::xmlser::FromXml(element, "breakpointId", breakpointId);
}

std::unique_ptr<sngxml::dom::Element> DeleteRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointId, "breakpointId").release()));
    return element;
}

DeleteReply::DeleteReply()
    : success(), error()
{
}

DeleteReply::DeleteReply(sngxml::dom::Element* element)
    : success(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> DeleteReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

DepthRequest::DepthRequest()
{
}

DepthRequest::DepthRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> DepthRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

DepthReply::DepthReply()
    : success(), depth(), error()
{
}

DepthReply::DepthReply(sngxml::dom::Element* element)
    : success(), depth(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "depth", depth);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> DepthReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(depth, "depth").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

FramesRequest::FramesRequest()
    : lowFrame(), highFrame()
{
}

FramesRequest::FramesRequest(sngxml::dom::Element* element)
    : lowFrame(), highFrame()
{
    sngxml::xmlser::FromXml(element, "lowFrame", lowFrame);
    sngxml::xmlser::FromXml(element, "highFrame", highFrame);
}

std::unique_ptr<sngxml::dom::Element> FramesRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(lowFrame, "lowFrame").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(highFrame, "highFrame").release()));
    return element;
}

FramesReply::FramesReply()
    : success(), frames(), error()
{
}

FramesReply::FramesReply(sngxml::dom::Element* element)
    : success(), frames(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "frames", frames);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> FramesReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(frames, "frames").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

EvaluateRequest::EvaluateRequest()
    : expression()
{
}

EvaluateRequest::EvaluateRequest(sngxml::dom::Element* element)
    : expression()
{
    sngxml::xmlser::FromXml(element, "expression", expression);
}

std::unique_ptr<sngxml::dom::Element> EvaluateRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expression, "expression").release()));
    return element;
}

EvaluateReply::EvaluateReply()
    : success(), result(), error()
{
}

EvaluateReply::EvaluateReply(sngxml::dom::Element* element)
    : success(), result(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "result", result);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> EvaluateReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(result, "result").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

CountRequest::CountRequest()
    : expression()
{
}

CountRequest::CountRequest(sngxml::dom::Element* element)
    : expression()
{
    sngxml::xmlser::FromXml(element, "expression", expression);
}

std::unique_ptr<sngxml::dom::Element> CountRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expression, "expression").release()));
    return element;
}

CountReply::CountReply()
    : success(), count(), error()
{
}

CountReply::CountReply(sngxml::dom::Element* element)
    : success(), count(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "count", count);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> CountReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(count, "count").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

EvaluateChildRequest::EvaluateChildRequest()
    : expression(), start(), count()
{
}

EvaluateChildRequest::EvaluateChildRequest(sngxml::dom::Element* element)
    : expression(), start(), count()
{
    sngxml::xmlser::FromXml(element, "expression", expression);
    sngxml::xmlser::FromXml(element, "start", start);
    sngxml::xmlser::FromXml(element, "count", count);
}

std::unique_ptr<sngxml::dom::Element> EvaluateChildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expression, "expression").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(start, "start").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(count, "count").release()));
    return element;
}

EvaluateChildReply::EvaluateChildReply()
    : success(), results(), error()
{
}

EvaluateChildReply::EvaluateChildReply(sngxml::dom::Element* element)
    : success(), results(), error()
{
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "results", results);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> EvaluateChildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(results, "results").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

TargetRunningRequest::TargetRunningRequest()
{
}

TargetRunningRequest::TargetRunningRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> TargetRunningRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

TargetRunningReply::TargetRunningReply()
{
}

TargetRunningReply::TargetRunningReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> TargetRunningReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

TargetInputRequest::TargetInputRequest()
{
}

TargetInputRequest::TargetInputRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> TargetInputRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

TargetInputReply::TargetInputReply()
    : line(), eof()
{
}

TargetInputReply::TargetInputReply(sngxml::dom::Element* element)
    : line(), eof()
{
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "eof", eof);
}

std::unique_ptr<sngxml::dom::Element> TargetInputReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(eof, "eof").release()));
    return element;
}

TargetOutputRequest::TargetOutputRequest()
    : handle(), output()
{
}

TargetOutputRequest::TargetOutputRequest(sngxml::dom::Element* element)
    : handle(), output()
{
    sngxml::xmlser::FromXml(element, "handle", handle);
    sngxml::xmlser::FromXml(element, "output", output);
}

std::unique_ptr<sngxml::dom::Element> TargetOutputRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(handle, "handle").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(output, "output").release()));
    return element;
}

TargetOutputReply::TargetOutputReply()
{
}

TargetOutputReply::TargetOutputReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> TargetOutputReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

LogDebugMessageRequest::LogDebugMessageRequest()
    : logMessage()
{
}

LogDebugMessageRequest::LogDebugMessageRequest(sngxml::dom::Element* element)
    : logMessage()
{
    sngxml::xmlser::FromXml(element, "logMessage", logMessage);
}

std::unique_ptr<sngxml::dom::Element> LogDebugMessageRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(logMessage, "logMessage").release()));
    return element;
}

LogDebugMessageReply::LogDebugMessageReply()
{
}

LogDebugMessageReply::LogDebugMessageReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> LogDebugMessageReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

KillRequest::KillRequest()
{
}

KillRequest::KillRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KillRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

KillReply::KillReply()
{
}

KillReply::KillReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KillReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

