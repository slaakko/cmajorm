#ifndef BUILDSERVERMESSAGE_HPP_9B23CE3BD05810A5126B0F5B6C9EDFB50250F5D2
#define BUILDSERVERMESSAGE_HPP_9B23CE3BD05810A5126B0F5B6C9EDFB50250F5D2
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

class StopRequest
{
public:
    StopRequest();
    StopRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class StopReply
{
public:
    StopReply();
    StopReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class KeepAliveRequest
{
public:
    KeepAliveRequest();
    KeepAliveRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class KeepAliveReply
{
public:
    KeepAliveReply();
    KeepAliveReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class BuildRequest
{
public:
    BuildRequest();
    BuildRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
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

class CompileError
{
public:
    CompileError();
    CompileError(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string message;
    std::string project;
    std::string file;
    int32_t line;
    int32_t scol;
    int32_t ecol;
};

class BuildReply
{
public:
    BuildReply();
    BuildReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool requestValid;
    std::string requestErrorMessage;
    std::string logException;
    std::string time;
    bool success;
    std::vector<CompileError> errors;
};

class LogMessageRequest
{
public:
    LogMessageRequest();
    LogMessageRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string message;
};

class LogMessageReply
{
public:
    LogMessageReply();
    LogMessageReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool ok;
};

class ProgressMessage
{
public:
    ProgressMessage();
    ProgressMessage(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class GenericErrorReply
{
public:
    GenericErrorReply();
    GenericErrorReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string error;
};

class CacheModuleRequest
{
public:
    CacheModuleRequest();
    CacheModuleRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string projectName;
    std::string projectFilePath;
    std::string backend;
    std::string config;
};

class CacheModuleReply
{
public:
    CacheModuleReply();
    CacheModuleReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string error;
};

class SourceLocation
{
public:
    SourceLocation();
    SourceLocation(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string file;
    int32_t line;
    int32_t scol;
    int32_t ecol;
};

class GetDefinitionRequest
{
public:
    GetDefinitionRequest();
    GetDefinitionRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string projectName;
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string identifier;
    SourceLocation identifierLocation;
};

class GetDefinitionReply
{
public:
    GetDefinitionReply();
    GetDefinitionReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    SourceLocation definitionLocation;
    bool ok;
    std::string error;
};

#endif // BUILDSERVERMESSAGE_HPP_9B23CE3BD05810A5126B0F5B6C9EDFB50250F5D2
