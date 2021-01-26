#ifndef CODECOMPLETIONSERVERMESSAGE_HPP_012CDE82AB35DD9B6579D9379A99DF1EAC6DCD1F
#define CODECOMPLETIONSERVERMESSAGE_HPP_012CDE82AB35DD9B6579D9379A99DF1EAC6DCD1F
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

class CodeCompletionRequest
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

class CodeCompletionReply
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

class LoadEditModuleRequest : public CodeCompletionRequest
{
public:
    LoadEditModuleRequest();
    LoadEditModuleRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
};

class LoadEditModuleReply : public CodeCompletionReply
{
public:
    LoadEditModuleReply();
    LoadEditModuleReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
};

class StopRequest : public CodeCompletionRequest
{
public:
    StopRequest();
    StopRequest(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
public:
};

class StopReply : public CodeCompletionReply
{
public:
    StopReply();
    StopReply(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const override;
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

#endif // CODECOMPLETIONSERVERMESSAGE_HPP_012CDE82AB35DD9B6579D9379A99DF1EAC6DCD1F
