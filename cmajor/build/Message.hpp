// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_MESSAGE_INCLUDED
#define CMAJOR_BUILD_MESSAGE_INCLUDED
#include <cmajor/build/Connection.hpp>
#include <sngjson/json/JsonImport.hpp>
#include <sngjson/json/JsonExport.hpp>
#include <stdexcept>
#include <unordered_map>

namespace cmajor { namespace build {

class MessageHandler;

class BUILD_API MessageBase
{
public:
    virtual ~MessageBase();
    virtual const char* Id() const = 0;
    virtual void DispatchTo(MessageHandler& handler) = 0;
};

template<typename MessageBodyT>
    requires sngjson::json::JsonExportable<MessageBodyT> && sngjson::json::JsonConstructible<MessageBodyT>
struct Message : public MessageBase
{
public:
    Message() : body()
    {
    }
    Message(JsonObject* messageObject) : body()
    {
        sngjson::json::FromJson(messageObject, body);
    }
    void SendTo(Connection& connection)
    {
        std::unique_ptr<JsonValue> messageValue = sngjson::json::ToJson(body);
        if (messageValue.get()->Type() == JsonValueType::object)
        {
            JsonObject* messageObject = static_cast<JsonObject*>(messageValue.get());
            messageObject->AddField(U"id", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(Id()))));
            connection.Send(messageObject);
        }
        else
        {
            throw std::runtime_error("could not send: invalid message (not JSON object)");
        }
    }
    MessageBodyT body;
};

class BUILD_API MessageCreator
{
public:
    virtual ~MessageCreator();
    virtual std::unique_ptr<MessageBase> CreateMessage(JsonObject* messageObject) = 0;
};

class BUILD_API MessageFactory
{
public:
    MessageFactory();
    MessageFactory(const MessageFactory&) = delete;
    MessageFactory(MessageFactory&&) = delete;
    MessageFactory& operator=(const MessageFactory&) = delete;
    MessageFactory& operator=(MessageFactory&&) = delete;
    static void Init();
    static void Done();
    static MessageFactory& Instance() { return *instance; }
    void Register(const char* messageId, MessageCreator* messageCreator);
    std::unique_ptr<MessageBase> CreateMessage(JsonObject* messageObject);
private:
    static std::unique_ptr<MessageFactory> instance;
    std::unordered_map<std::string, MessageCreator*> creatorMap;
    std::vector<std::unique_ptr<MessageCreator>> creators;
};

template<typename T>
class ConcreteMessageCreator : public MessageCreator
{
public:
    std::unique_ptr<MessageBase> CreateMessage(JsonObject* messagObject) override
    {
        std::unique_ptr<MessageBase> msg(new T(messagObject));
        return msg;
    }
};

BUILD_API void MessageInit();
BUILD_API void MessageDone();

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_MESSAGE_INCLUDED
