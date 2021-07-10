#ifndef CODECOMPLETIONSERVERMESSAGE_HPP_012CDE82AB35DD9B6579D9379A99DF1EAC6DCD1F
#define CODECOMPLETIONSERVERMESSAGE_HPP_012CDE82AB35DD9B6579D9379A99DF1EAC6DCD1F
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

class MSG_API CodeCompletionRequest
{
public:
    CodeCompletionRequest();
    CodeCompletionRequest(sngxml::dom::Element* element);
    virtual ~CodeCompletionRequest();
    virtual std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    time_point created;
    time_point received;
};

class MSG_API CodeCompletionReply
{
public:
    CodeCompletionReply();
    CodeCompletionReply(sngxml::dom::Element* element);
    virtual ~CodeCompletionReply();
    virtual std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    time_point requestCreated;
    time_point requestReceived;
    time_point created;
};

class MSG_API LoadEditModuleRequest : public CodeCompletionRequest
{
public:
    LoadEditModuleRequest();
    LoadEditModuleRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    std::string projectFilePath;
    std::string backend;
    std::string config;
};

class MSG_API LoadEditModuleReply : public CodeCompletionReply
{
public:
    LoadEditModuleReply();
    LoadEditModuleReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    std::string key;
    std::string result;
    std::string error;
    time_point startLoading;
    time_point startParsing;
    time_point endParsing;
    time_point endLoading;
};

class MSG_API ResetEditModuleCacheRequest : public CodeCompletionRequest
{
public:
    ResetEditModuleCacheRequest();
    ResetEditModuleCacheRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
};

class MSG_API ResetEditModuleCacheReply : public CodeCompletionReply
{
public:
    ResetEditModuleCacheReply();
    ResetEditModuleCacheReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    std::string result;
    std::string error;
};

class MSG_API ParseSourceRequest : public CodeCompletionRequest
{
public:
    ParseSourceRequest();
    ParseSourceRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    std::u32string sourceCode;
};

class MSG_API ParseSourceReply : public CodeCompletionReply
{
public:
    ParseSourceReply();
    ParseSourceReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    bool ok;
    std::string error;
    std::vector<std::string> errors;
    bool synchronized;
    std::string cursorContainer;
    std::vector<int32_t> ruleContext;
    time_point startParsing;
    time_point endParsing;
};

class MSG_API GetCCListRequest : public CodeCompletionRequest
{
public:
    GetCCListRequest();
    GetCCListRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    std::u32string ccText;
    std::vector<int32_t> ruleContext;
    std::u32string cursorLine;
};

class MSG_API GetCCListReply : public CodeCompletionReply
{
public:
    GetCCListReply();
    GetCCListReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    bool ok;
    std::string error;
    std::string ccList;
    time_point startGetCCList;
    time_point endGetCCList;
};

class MSG_API GetParamHelpListRequest : public CodeCompletionRequest
{
public:
    GetParamHelpListRequest();
    GetParamHelpListRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    std::string projectFilePath;
    std::string backend;
    std::string config;
    std::string sourceFilePath;
    int32_t symbolIndex;
};

class MSG_API GetParamHelpListReply : public CodeCompletionReply
{
public:
    GetParamHelpListReply();
    GetParamHelpListReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
    bool ok;
    std::string error;
    std::string list;
    time_point startGetParamHelpList;
    time_point endGetParamHelpList;
};

class MSG_API StopCCRequest : public CodeCompletionRequest
{
public:
    StopCCRequest();
    StopCCRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
};

class MSG_API StopCCReply : public CodeCompletionReply
{
public:
    StopCCReply();
    StopCCReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
};

class MSG_API KeepAliveCCRequest
{
public:
    KeepAliveCCRequest();
    KeepAliveCCRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

class MSG_API KeepAliveCCReply
{
public:
    KeepAliveCCReply();
    KeepAliveCCReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
};

#endif // CODECOMPLETIONSERVERMESSAGE_HPP_012CDE82AB35DD9B6579D9379A99DF1EAC6DCD1F
