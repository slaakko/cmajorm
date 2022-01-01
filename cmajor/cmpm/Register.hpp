// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPM_REGISTER_INCLUDED
#define CMAJOR_CMPM_REGISTER_INCLUDED
#include <cmpm/Api.hpp>

namespace cmpm {

const int helloPmsRequestClassId = 100;
const int helloPmsReplyClassId = 101;
const int getFreePortNumberRequestClassId = 102;
const int getFreePortNumberReplyClassId = 103;
const int extendPortLeaseRequestClassId = 104;
const int extendPortLeaseReplyClassId = 105;
const int stopPortMapServerRequestClassId = 106;
const int stopPortMapServerReplyClassId = 107;
const int portLeaseClassId = 108;
const int viewPortLeaseRequestClassId = 109;
const int viewPortLeaseReplyClassId = 110;

CMPM_API void Register();

} // namnespace cmpm

#endif // CMAJOR_CMPM_REGISTER_INCLUDED

