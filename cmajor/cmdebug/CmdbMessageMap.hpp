// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CMDB_MESSAGE_MAP_INCLUDED
#define CMAJOR_DEBUG_CMDB_MESSAGE_MAP_INCLUDED
#include <string>
#include <map>

namespace cmajor { namespace debug {

enum class MessageKind
{
    none, startRequest, startReply, stopRequest, stopReply, genericErrorReply, continueRequest, continueReply, nextRequest, nextReply, stepRequest, stepReply,
    breakRequest, breakReply, deleteRequest, deleteReply,
    targetRunningRequest, targetRunningReply, targetInputRequest, targetInputReply, targetOutputRequest, targetOutputReply, logMessageRequest, logMessageReply
};

class MessageMap
{
public:
    MessageMap();
    MessageKind GetMessageKind(const std::string& messageKindStr);
private:
    std::map<std::string, MessageKind> map;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CMDB_MESSAGE_MAP_INCLUDED
