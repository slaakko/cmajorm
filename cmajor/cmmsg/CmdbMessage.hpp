#ifndef CMDBMESSAGE_HPP_9A6DA1BD20F12675B0E78EEAF8B950F3D0DDB92E
#define CMDBMESSAGE_HPP_9A6DA1BD20F12675B0E78EEAF8B950F3D0DDB92E
#include <cmajor/cmmsg/MsgApi.hpp>
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

class MSG_API Location
{
public:
    Location();
    Location(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string func;
    std::string file;
    int32_t line;
    int32_t scol;
    int32_t ecol;
    int32_t level;
};

class MSG_API Type
{
public:
    Type();
    Type(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string name;
    std::string id;
};

class MSG_API Result
{
public:
    Result();
    Result(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    Type staticType;
    Type dynamicType;
    bool initialized;
    std::string value;
};

class MSG_API ChildResult
{
public:
    ChildResult();
    ChildResult(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string expr;
    std::string name;
    std::string type;
    std::string dynType;
    std::string value;
    int32_t count;
};

class MSG_API TargetState
{
public:
    TargetState();
    TargetState(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string stopReason;
    std::string exitCode;
    std::string breakpointId;
    std::string signalName;
    std::string signalMeaning;
};

class MSG_API SourceLoc
{
public:
    SourceLoc();
    SourceLoc(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string path;
    int32_t line;
};

class MSG_API BreakpointInfo
{
public:
    BreakpointInfo();
    BreakpointInfo(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    std::string breakpointId;
    int32_t numInsts;
    Location location;
    std::string error;
};

class MSG_API StartDebugRequest
{
public:
    StartDebugRequest();
    StartDebugRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::vector<SourceLoc> breakpointLocations;
};

class MSG_API StartDebugReply
{
public:
    StartDebugReply();
    StartDebugReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    Location location;
    TargetState state;
    std::vector<BreakpointInfo> breakpointInfos;
    std::string error;
};

class MSG_API StopDebugRequest
{
public:
    StopDebugRequest();
    StopDebugRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API StopDebugReply
{
public:
    StopDebugReply();
    StopDebugReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API GenericDebugErrorReply
{
public:
    GenericDebugErrorReply();
    GenericDebugErrorReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string errorMessage;
};

class MSG_API ContinueRequest
{
public:
    ContinueRequest();
    ContinueRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API ContinueReply
{
public:
    ContinueReply();
    ContinueReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

class MSG_API NextRequest
{
public:
    NextRequest();
    NextRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API NextReply
{
public:
    NextReply();
    NextReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

class MSG_API StepRequest
{
public:
    StepRequest();
    StepRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API StepReply
{
public:
    StepReply();
    StepReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

class MSG_API FinishRequest
{
public:
    FinishRequest();
    FinishRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API FinishReply
{
public:
    FinishReply();
    FinishReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

class MSG_API UntilRequest
{
public:
    UntilRequest();
    UntilRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    SourceLoc sourceLoc;
};

class MSG_API UntilReply
{
public:
    UntilReply();
    UntilReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

class MSG_API BreakRequest
{
public:
    BreakRequest();
    BreakRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    SourceLoc breakpointLocation;
};

class MSG_API BreakReply
{
public:
    BreakReply();
    BreakReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    BreakpointInfo breakpointInfo;
};

class MSG_API DeleteRequest
{
public:
    DeleteRequest();
    DeleteRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string breakpointId;
};

class MSG_API DeleteReply
{
public:
    DeleteReply();
    DeleteReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    std::string error;
};

class MSG_API DepthRequest
{
public:
    DepthRequest();
    DepthRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API DepthReply
{
public:
    DepthReply();
    DepthReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    int32_t depth;
    std::string error;
};

class MSG_API FramesRequest
{
public:
    FramesRequest();
    FramesRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    int32_t lowFrame;
    int32_t highFrame;
};

class MSG_API FramesReply
{
public:
    FramesReply();
    FramesReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    std::vector<Location> frames;
    std::string error;
};

class MSG_API EvaluateRequest
{
public:
    EvaluateRequest();
    EvaluateRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string expression;
};

class MSG_API EvaluateReply
{
public:
    EvaluateReply();
    EvaluateReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    Result result;
    std::string error;
};

class MSG_API CountRequest
{
public:
    CountRequest();
    CountRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string expression;
};

class MSG_API CountReply
{
public:
    CountReply();
    CountReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    int32_t count;
    std::string error;
};

class MSG_API EvaluateChildRequest
{
public:
    EvaluateChildRequest();
    EvaluateChildRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string expression;
    int32_t start;
    int32_t count;
};

class MSG_API EvaluateChildReply
{
public:
    EvaluateChildReply();
    EvaluateChildReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool success;
    std::vector<ChildResult> results;
    std::string error;
};

class MSG_API TargetRunningRequest
{
public:
    TargetRunningRequest();
    TargetRunningRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API TargetRunningReply
{
public:
    TargetRunningReply();
    TargetRunningReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API TargetInputRequest
{
public:
    TargetInputRequest();
    TargetInputRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API TargetInputReply
{
public:
    TargetInputReply();
    TargetInputReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string line;
    bool eof;
};

class MSG_API TargetOutputRequest
{
public:
    TargetOutputRequest();
    TargetOutputRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    int32_t handle;
    std::string output;
};

class MSG_API TargetOutputReply
{
public:
    TargetOutputReply();
    TargetOutputReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API LogDebugMessageRequest
{
public:
    LogDebugMessageRequest();
    LogDebugMessageRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string logMessage;
};

class MSG_API LogDebugMessageReply
{
public:
    LogDebugMessageReply();
    LogDebugMessageReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API KillRequest
{
public:
    KillRequest();
    KillRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API KillReply
{
public:
    KillReply();
    KillReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

#endif // CMDBMESSAGE_HPP_9A6DA1BD20F12675B0E78EEAF8B950F3D0DDB92E
