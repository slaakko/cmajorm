#ifndef HPP_1D3CEDFE37CD9403F7EA26C07CE160FC9C3CB8D2
#define HPP_1D3CEDFE37CD9403F7EA26C07CE160FC9C3CB8D2
#include <soulng/util/Json.hpp>
#include <string>
#include <vector>
#include <memory>

struct StopRequest
{
    StopRequest();
    StopRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct KeepAliveRequest
{
    KeepAliveRequest();
    KeepAliveRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct BuildRequest
{
    BuildRequest();
    BuildRequest(soulng::util::JsonValue* value);
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
    BuildReply(soulng::util::JsonValue* value);
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
    LogMessageRequest(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string message;
};

struct LogMessageReply
{
    LogMessageReply();
    LogMessageReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    bool ok;
};

struct ProgressMessage
{
    ProgressMessage();
    ProgressMessage(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
};

struct GenericErrorReply
{
    GenericErrorReply();
    GenericErrorReply(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string messageKind;
    std::string error;
};

#endif // HPP_1D3CEDFE37CD9403F7EA26C07CE160FC9C3CB8D2
