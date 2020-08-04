// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CMDB_SESSION_INCLUDED
#define CMAJOR_DEBUG_CMDB_SESSION_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/CodeFormatter.hpp>

namespace cmajor { namespace debug {

using namespace soulng::util;

class Debugger;

DEBUG_API void SetCmdbSessionPort(int port_);
DEBUG_API void StartCmdbSession(const std::string& cmdbSessionFilePath, CodeFormatter& formatter, Debugger* debugger, bool verbose);
DEBUG_API void StopCmdbSession();
DEBUG_API void CmdbSessionInit();
DEBUG_API void CmdbSessionDone();

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CMDB_SESSION_INCLUDED
