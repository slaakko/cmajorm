// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/DebugMsg.hpp>
#include <system-x/kernel/OsApi.hpp>
#include <soulng/util/MemoryReader.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

std::string MsgName(int msgId)
{
    switch (msgId)
    {
        case systemScreenQuitMessageId:
        {
            return "system.screen.quit";
        }
        case systemScreenKeyPressedMessageId:
        {
            return "system.screen.key.pressed";
        }
        case systemScreenWriteScreenMessageId:
        {
            return "system.screen.write.screen";
        }
        case systemScreenTimerMessageId:
        {
            return "system.screen.timer";
        }
        case tabCompletionInitMessageId:
        {
            return "tab.completion.init";
        }
        case tabCompletionExitMessageId:
        {
            return "tab.completion.exit";
        }
        case tabCompletionRequestMessageId:
        {
            return "tab.completion.request";
        }
        case tabCompletionReplyMessageId:
        {
            return "tab.completion.reply";
        }
        case tabCompletionErrorReplyMessageId:
        {
            return "tab.completion.error";
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
    int32_t x = reader.ReadInt();
    int32_t y = reader.ReadInt();
    int32_t w = reader.ReadInt();
    int32_t h = reader.ReadInt();
    int32_t invalidateKind = reader.ReadInt();
    if (x == -1 && y == -1 && w == -1 && h == -1)
    {
        params.append(".default");
    }
    else
    {
        params.append(".x=").append(std::to_string(x));
        params.append(".y=").append(std::to_string(y));
        params.append(".w=").append(std::to_string(w));
        params.append(".h=").append(std::to_string(h));
    }
    params.append(".invalidate=" + std::to_string(invalidateKind));
    return params;
}

std::string TimerMessageParams(MemoryReader& reader)
{
    std::string params;
    params.append(".timerId=").append(std::to_string(reader.ReadInt()));
    return params;
}

std::string ScreenMsgContentStr(int msgId, MemoryReader& reader)
{
    std::string contentStr;
    int targetWindowHandle = reader.ReadInt();
    if (targetWindowHandle == -1)
    {
        contentStr.append("window=all");
    }
    else
    {
        contentStr.append("window=").append(std::to_string(targetWindowHandle));
    }
    switch (msgId)
    {
        case systemScreenKeyPressedMessageId:
        {
            contentStr.append(KeyPressedMessageParams(reader));
            break;
        }
        case systemScreenWriteScreenMessageId:
        {
            contentStr.append(WriteScreenMessageParams(reader));
            break;
        }
        case systemScreenTimerMessageId:
        {
            contentStr.append(TimerMessageParams(reader));
            break;
        }
    }
    return contentStr;
}

std::string CompletionMsgContentStr(int msgId, MemoryReader& reader)
{
    std::string contentStr;
    switch (msgId)
    {
        case tabCompletionInitMessageId:
        {
            break;
        }
        case tabCompletionExitMessageId:
        {
            break;
        }
        case tabCompletionRequestMessageId:
        {
            std::string cwd = reader.ReadString();
            std::string line = reader.ReadString();
            int32_t pos = reader.ReadInt();
            contentStr.append("cwd='").append(cwd).append("'").append(".line='").append(line).append("'").append(".pos=").append(std::to_string(pos));
            break;
        }
        case tabCompletionReplyMessageId:
        {
            int32_t success = reader.ReadInt();
            contentStr.append("success=").append(std::to_string(success));
            if (success)
            {
                int32_t numCompletions = reader.ReadInt();
                contentStr.append(".completions=").append(std::to_string(numCompletions));
            }
            break;
        }
        case tabCompletionErrorReplyMessageId:
        {
            int32_t success = reader.ReadInt();
            contentStr.append("success=").append(std::to_string(success));
            std::string errorMsg = reader.ReadString();
            contentStr.append(".error='").append(std::to_string(success)).append("'");
            break;
        }
    }
    return contentStr;
}

std::string MsgContentStr(int msgId, MemoryReader& reader)
{
    std::string contentStr;
    if (msgId >= firstScreenMessageId && msgId <= lastScreenMessageId)
    {
        contentStr = ScreenMsgContentStr(msgId, reader);
    }
    else if (msgId >= firstCompletionMessageId && msgId <= lastCompletionMessageId)
    {
        contentStr = CompletionMsgContentStr(msgId, reader);
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
