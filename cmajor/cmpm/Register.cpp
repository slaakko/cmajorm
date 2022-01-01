// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmpm/Register.hpp>
#include <cmpm/PortMapMessage.hpp>

namespace cmpm {

void Register()
{
    HelloPmsRequest::Register(helloPmsRequestClassId);
    HelloPmsReply::Register(helloPmsReplyClassId);
    GetFreePortNumberRequest::Register(getFreePortNumberRequestClassId);
    GetFreePortNumberReply::Register(getFreePortNumberReplyClassId);
    ExtendPortLeaseRequest::Register(extendPortLeaseRequestClassId);
    ExtendPortLeaseReply::Register(extendPortLeaseReplyClassId);
    StopPortMapServerRequest::Register(stopPortMapServerRequestClassId);
    StopPortMapServerReply::Register(stopPortMapServerReplyClassId);
    PortLease::Register(portLeaseClassId);
    ViewPortLeaseRequest::Register(viewPortLeaseRequestClassId);
    ViewPortLeaseReply::Register(viewPortLeaseReplyClassId);
}

} // namespace cmpm
