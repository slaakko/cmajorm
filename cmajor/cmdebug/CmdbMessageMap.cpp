// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/CmdbMessageMap.hpp>

namespace cmajor { namespace debug {

MessageMap::MessageMap()
{
    map["startRequest"] = MessageKind::startRequest;
    map["startReply"] = MessageKind::startReply;
    map["stopRequest"] = MessageKind::stopRequest;
    map["stopReply"] = MessageKind::stopReply;
    map["genericErrorReply"] = MessageKind::genericErrorReply;
    map["continueRequest"] = MessageKind::continueRequest;
    map["continueReply"] = MessageKind::continueReply;
    map["nextRequest"] = MessageKind::nextRequest;
    map["nextReply"] = MessageKind::nextReply;
    map["stepRequest"] = MessageKind::stepRequest;
    map["stepReply"] = MessageKind::stepReply;
    map["finishRequest"] = MessageKind::finishRequest;
    map["finishReply"] = MessageKind::finishReply;
    map["untilRequest"] = MessageKind::untilRequest;
    map["untilReply"] = MessageKind::untilReply;
    map["breakRequest"] = MessageKind::breakRequest;
    map["breakReply"] = MessageKind::breakReply;
    map["deleteRequest"] = MessageKind::deleteRequest;
    map["deleteReply"] = MessageKind::deleteReply;
    map["depthRequest"] = MessageKind::depthRequest;
    map["depthReply"] = MessageKind::depthReply;
    map["framesRequest"] = MessageKind::framesRequest;
    map["framesReply"] = MessageKind::framesReply;
    map["countRequest"] = MessageKind::countRequest;
    map["countReply"] = MessageKind::countReply;
    map["evaluateChildRequest"] = MessageKind::evaluateChildRequest;
    map["evaluateChildReply"] = MessageKind::evaluateChildReply;
    map["targetRunningRequest"] = MessageKind::targetRunningRequest;
    map["targetRunningReply"] = MessageKind::targetRunningReply;
    map["targetInputRequest"] = MessageKind::targetInputRequest;
    map["targetInputReply"] = MessageKind::targetInputReply;
    map["targetOutputRequest"] = MessageKind::targetOutputRequest;
    map["targetOutputReply"] = MessageKind::targetOutputReply;
    map["logMessageRequest"] = MessageKind::logMessageRequest;
    map["logMessageReply"] = MessageKind::logMessageReply;
}

MessageKind MessageMap::GetMessageKind(const std::string& messageKindStr)
{
    auto it = map.find(messageKindStr);
    if (it != map.cend())
    {
        return it->second;
    }
    return MessageKind::none;
}

} } // namespace cmajor::debug
