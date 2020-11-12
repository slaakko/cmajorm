#ifndef HPP_9A6473C10E9B8484FB5E3A9E7350059CF7F315F8
#define HPP_9A6473C10E9B8484FB5E3A9E7350059CF7F315F8
#include <soulng/util/Json.hpp>
#include <string>
#include <vector>
#include <memory>

struct StopRequest
{
    StopRequest();
    StopRequest(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct StopReply
{
    StopReply();
    StopReply(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct KeepAliveRequest
{
    KeepAliveRequest();
    KeepAliveRequest(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct KeepAliveReply
{
    KeepAliveReply();
    KeepAliveReply(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct BuildRequest
{
    BuildRequest();
    BuildRequest(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string backend;
    std::string config;
    std::string optimizationLevel;
    std::string filePath;
    std::vector<std::string> defines;
    std::string numBuildThreads;
    std::string toolChain;
    bool verbose;
    bool quiet;
    bool clean;
    bool strictNothrow;
    bool emitIR;
    bool emitOptIR;
    bool linkWithDebugRuntime;
    bool linkUsingMsLink;
    bool rebuild;
    bool buildAllDependencies;
    bool forceDebugInfo;
    bool forceNoDebugInfo;
    bool disableModuleCache;
    bool singleThreadedCompile;
    bool disableCodeGen;
    bool justMyCode;
};

struct BuildReply
{
    BuildReply();
    BuildReply(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool requestValid;
    std::string requestErrorMessage;
    std::string logException;
    std::string time;
    bool success;
    std::string error;
};

struct LogMessageRequest
{
    LogMessageRequest();
    LogMessageRequest(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string message;
};

struct LogMessageReply
{
    LogMessageReply();
    LogMessageReply(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool ok;
};

struct ProgressMessage
{
    ProgressMessage();
    ProgressMessage(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct GenericErrorReply
{
    GenericErrorReply();
    GenericErrorReply(soulng::util::JsonValue* __json_value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string error;
};

#endif // HPP_9A6473C10E9B8484FB5E3A9E7350059CF7F315F8
