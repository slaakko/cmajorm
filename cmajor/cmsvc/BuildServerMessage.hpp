#ifndef BUILDSERVERMESSAGE_HPP_9B23CE3BD05810A5126B0F5B6C9EDFB50250F5D2
#define BUILDSERVERMESSAGE_HPP_9B23CE3BD05810A5126B0F5B6C9EDFB50250F5D2
#include <cmajor/cmsvc/ServiceApi.hpp>
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

class CMSVC_API StopBuildRequest
{
public:
    StopBuildRequest();
    StopBuildRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMSVC_API StopBuildReply
{
public:
    StopBuildReply();
    StopBuildReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMSVC_API KeepAliveBuildRequest
{
public:
    KeepAliveBuildRequest();
    KeepAliveBuildRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMSVC_API KeepAliveBuildReply
{
public:
    KeepAliveBuildReply();
    KeepAliveBuildReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMSVC_API BuildRequest
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

class CMSVC_API CompileError
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

class CMSVC_API BuildReply
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

class CMSVC_API LogBuildMessageRequest
{
public:
    LogBuildMessageRequest();
    LogBuildMessageRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string message;
};

class CMSVC_API LogBuildMessageReply
{
public:
    LogBuildMessageReply();
    LogBuildMessageReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool ok;
};

class CMSVC_API BuildProgressMessage
{
public:
    BuildProgressMessage();
    BuildProgressMessage(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class CMSVC_API GenericBuildErrorReply
{
public:
    GenericBuildErrorReply();
    GenericBuildErrorReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string error;
};

class CMSVC_API CacheModuleRequest
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

class CMSVC_API CacheModuleReply
{
public:
    CacheModuleReply();
    CacheModuleReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string error;
};

class CMSVC_API DefinitionSourceLocation
{
public:
    DefinitionSourceLocation();
    DefinitionSourceLocation(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string file;
    int32_t line;
    int32_t scol;
    int32_t ecol;
};

class CMSVC_API GetDefinitionRequest
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
    DefinitionSourceLocation identifierLocation;
};

class CMSVC_API GetDefinitionReply
{
public:
    GetDefinitionReply();
    GetDefinitionReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    DefinitionSourceLocation definitionLocation;
    bool ok;
    std::string error;
};

#endif // BUILDSERVERMESSAGE_HPP_9B23CE3BD05810A5126B0F5B6C9EDFB50250F5D2
