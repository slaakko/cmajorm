// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/ClientChannel.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

ClientChannelUser::~ClientChannelUser()
{
}

void RunChannel(ClientChannel* channel, ClientChannelUser* user)
{
    try
    {
        channel->Run();
    }
    catch (const std::exception& ex)
    {
        user->ClientChannelError(ex.what());
    }
}

ClientChannel::ClientChannel(ClientChannelUser* user_, TcpSocket& socket_, int timeoutMs_) : user(user_), socket(socket_), timeoutMs(timeoutMs_)
{
    thread = std::thread{ RunChannel, this, user };
}

void ClientChannel::Run()
{
    try
    {
        std::unique_ptr<JsonValue> message = GetMessage();
        while (message)
        {
            std::string messageStr = message->ToString();
            Write(socket, messageStr);
            std::string replyStr = ReadStr(socket);
            JsonLexer lexer(ToUtf32(replyStr), "", 0);
            std::unique_ptr<JsonValue> reply = JsonParser::Parse(lexer);
            user->ProcessReceivedClientChannelMessage(reply.get());
            message = GetMessage();
        }
    }
    catch (const std::exception& ex)
    {
        user->ClientChannelError(ex.what());
    }
}

std::unique_ptr<JsonValue> ClientChannel::GetMessage()
{
    std::unique_lock<std::mutex> lock(mtx);
    if (!messageEnqueued.wait_for(lock, std::chrono::milliseconds{ timeoutMs }, [this] { return !messageQueue.empty(); }))
    {
        return user->GetIdleClientChannelMessage();
    }
    std::unique_ptr<JsonValue> message(std::move(messageQueue.front()));
    messageQueue.pop_front();
    return message;
}

void ClientChannel::SendMessage(JsonValue* message)
{
    std::lock_guard lock(mtx);
    messageQueue.push_back(std::unique_ptr<JsonValue>(message));
    messageEnqueued.notify_one();
}

ClientChannel::~ClientChannel()
{
    try
    {
        SendMessage(nullptr);
        thread.join();
    }
    catch (const std::exception& ex)
    {
        user->ClientChannelError(ex.what());
    }
}

} } // namespace cmajor::debug
