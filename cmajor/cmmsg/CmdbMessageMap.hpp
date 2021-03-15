// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MSG_CMDB_MESSAGE_MAP_INCLUDED
#define CMAJOR_MSG_CMDB_MESSAGE_MAP_INCLUDED
#include <cmajor/cmmsg/MsgApi.hpp>
#include <string>
#include <map>

namespace cmajor { namespace msg {

enum class MessageKind
{
    none, startRequest, startReply, stopRequest, stopReply, genericErrorReply, continueRequest, continueReply, nextRequest, nextReply, stepRequest, stepReply,
    finishRequest, finishReply, untilRequest, untilReply,
    breakRequest, breakReply, deleteRequest, deleteReply, depthRequest, depthReply, framesRequest, framesReply, 
    countRequest, countReply, evaluateChildRequest, evaluateChildReply, evaluateRequest, evaluateReply,
    targetRunningRequest, targetRunningReply, targetInputRequest, targetInputReply, targetOutputRequest, targetOutputReply, logMessageRequest, logMessageReply
};

class MSG_API MessageMap
{
public:
    MessageMap();
    MessageKind GetMessageKind(const std::string& messageKindStr);
private:
    std::map<std::string, MessageKind> map;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CMDB_MESSAGE_MAP_INCLUDED
