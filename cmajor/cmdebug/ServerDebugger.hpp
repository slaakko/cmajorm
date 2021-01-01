// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_SERVER_DEBUGGER_INCLUDED
#define CMAJOR_DEBUG_SERVER_DEBUGGER_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <string>
#include <vector>

namespace cmajor { namespace debug {

DEBUG_API void StartDebuggerServer(const std::string& executable, const std::vector<std::string>& args, bool verbose, bool breakOnThrow, const std::string& version,
    int port, bool log);
DEBUG_API void StopDebuggerServer();
DEBUG_API void RunDebuggerServer(const std::string& executable, const std::vector<std::string>& args, bool verbose, bool breakOnThrow, const std::string& version,
int port, bool log);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_SERVER_DEBUGGER_INCLUDED
