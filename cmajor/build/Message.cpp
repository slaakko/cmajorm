// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/Message.hpp>

namespace cmajor { namespace build {

MessageBase::~MessageBase()
{
}

MessageCreator::~MessageCreator()
{
}

MessageFactory::MessageFactory()
{
}

std::unique_ptr<MessageFactory> MessageFactory::instance;

void MessageFactory::Init()
{
    instance.reset(new MessageFactory());
}

void MessageFactory::Done()
{
    instance.reset();
}

void MessageFactory::Register(const char* messageId, MessageCreator* messageCreator)
{
    creatorMap[messageId] = messageCreator;
    creators.push_back(std::unique_ptr<MessageCreator>(messageCreator));
}

std::unique_ptr<MessageBase> MessageFactory::CreateMessage(soulng::util::JsonObject* messageObject)
{
    soulng::util::JsonValue* idField = messageObject->GetField(U"id");
    if (idField)
    {
        if (idField->Type() == soulng::util::JsonValueType::string)
        {
            soulng::util::JsonString* idStr = static_cast<soulng::util::JsonString*>(idField);
            std::string id = ToUtf8(idStr->Value());
            auto it = creatorMap.find(id);
            if (it != creatorMap.cend())
            {
                MessageCreator* creator = it->second;
                return creator->CreateMessage(messageObject);
            }
            else
            {
                throw std::runtime_error("message factory error: could not create message, no creator for id '" + id + "'");
            }
        }
        else
        {
            throw std::runtime_error("message factory error: could not create message, id field not a JSON string");
        }
    }
    else
    {
        throw std::runtime_error("message factory error: could not create message, id field missing");
    }
}

void MessageInit()
{
    MessageFactory::Init();
}

void MessageDone()
{
    MessageFactory::Done();
}

} } // namespace cmajor::build
