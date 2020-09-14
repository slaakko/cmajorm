#ifndef HPP_119B103F454BFA5EB7848C8361DFF12FF7084219
#define HPP_119B103F454BFA5EB7848C8361DFF12FF7084219
#include <soulng/util/Json.hpp>
#include <string>
#include <vector>
#include <memory>

struct Location
{
    Location();
    Location(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string func;
    std::string file;
    std::string line;
    std::string scol;
    std::string ecol;
    std::string level;
};

struct TargetState
{
    TargetState();
    TargetState(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string stopReason;
    std::string exitCode;
    std::string breakpointId;
    std::string signalName;
    std::string signalMeaning;
};

struct SourceLoc
{
    SourceLoc();
    SourceLoc(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string path;
    std::string line;
};

struct BreakpointInfo
{
    BreakpointInfo();
    BreakpointInfo(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    bool success;
    std::string breakpointId;
    std::string numInsts;
    Location location;
    std::string error;
};

struct StartRequest
{
    StartRequest();
    StartRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::vector<SourceLoc> breakpointLocations;
};

struct StartReply
{
    StartReply();
    StartReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool success;
    Location location;
    TargetState state;
    std::vector<BreakpointInfo> breakpointInfos;
    std::string error;
};

struct StopRequest
{
    StopRequest();
    StopRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct StopReply
{
    StopReply();
    StopReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct GenericErrorReply
{
    GenericErrorReply();
    GenericErrorReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string errorMessage;
};

struct ContinueRequest
{
    ContinueRequest();
    ContinueRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct ContinueReply
{
    ContinueReply();
    ContinueReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

struct NextRequest
{
    NextRequest();
    NextRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct NextReply
{
    NextReply();
    NextReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

struct StepRequest
{
    StepRequest();
    StepRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct StepReply
{
    StepReply();
    StepReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool success;
    Location location;
    TargetState state;
    std::string error;
};

struct BreakRequest
{
    BreakRequest();
    BreakRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    SourceLoc breakpointLocation;
};

struct BreakReply
{
    BreakReply();
    BreakReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    BreakpointInfo breakpointInfo;
};

struct DeleteRequest
{
    DeleteRequest();
    DeleteRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string breakpointId;
};

struct DeleteReply
{
    DeleteReply();
    DeleteReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool success;
    std::string error;
};

struct TargetRunningRequest
{
    TargetRunningRequest();
    TargetRunningRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct TargetRunningReply
{
    TargetRunningReply();
    TargetRunningReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct TargetInputRequest
{
    TargetInputRequest();
    TargetInputRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct TargetInputReply
{
    TargetInputReply();
    TargetInputReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string line;
    bool eof;
};

struct TargetOutputRequest
{
    TargetOutputRequest();
    TargetOutputRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string handle;
    std::string output;
};

struct TargetOutputReply
{
    TargetOutputReply();
    TargetOutputReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct LogMessageRequest
{
    LogMessageRequest();
    LogMessageRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string logMessage;
};

struct LogMessageReply
{
    LogMessageReply();
    LogMessageReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

#endif // HPP_119B103F454BFA5EB7848C8361DFF12FF7084219
