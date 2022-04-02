// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/DebugMsg.hpp>
#include <system-x/kernel/OsApi.hpp>
#include <soulng/util/MemoryReader.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

const int quitMessageId = 0;
const int keyPressedMessageId = 1;
const int writeScreenMessageId = 2;
const int timerMessageId = 3;

std::string MsgName(int msgId)
{
    switch (msgId)
    {
        case quitMessageId:
        {
            return "quit";
        }
        case keyPressedMessageId:
        {
            return "keyPressed";
        }
        case writeScreenMessageId:
        {
            return "writeScreen";
        }
        case timerMessageId:
        {
            return "timer";
        }
        default:
        {
            return "<unknown>";
        }
    }
}

std::string KeyPressedMessageParams(MemoryReader& reader)
{
    int key = reader.ReadInt();
    std::string params = ".key=" + OsKeyName(key);
    return params;
}

std::string WriteScreenMessageParams(MemoryReader& reader)
{
    std::string params;
    params.append(".x=").append(std::to_string(reader.ReadInt()));
    params.append(".y=").append(std::to_string(reader.ReadInt()));
    params.append(".w=").append(std::to_string(reader.ReadInt()));
    params.append(".h=").append(std::to_string(reader.ReadInt()));
    return params;
}

std::string TimerMessageParams(MemoryReader& reader)
{
    std::string params;
    params.append(".timerId=").append(std::to_string(reader.ReadInt()));
    return params;
}

std::string MsgContentStr(int msgId, MemoryReader& reader)
{
    std::string contentStr;
    int targetWindowHandle = reader.ReadInt();
    contentStr.append("window=").append(std::to_string(targetWindowHandle));
    switch (msgId)
    {
        case keyPressedMessageId:
        {
            contentStr.append(KeyPressedMessageParams(reader));
            break;
        }
        case writeScreenMessageId:
        {
            contentStr.append(WriteScreenMessageParams(reader));
            break;
        }
        case timerMessageId:
        {
            contentStr.append(TimerMessageParams(reader));
            break;
        }
    }
    return contentStr;
}

std::string GetMsgStr(const Msg& msg)
{
    MemoryReader reader(msg.Data().data(), msg.Data().size());
    int msgId = reader.ReadInt();
    std::string msgStr = "msg." + std::to_string(msgId) + "." + MsgName(msgId) + "(" + MsgContentStr(msgId, reader) + ")";
    return msgStr;
}

} // namespace cmsx::kernel
