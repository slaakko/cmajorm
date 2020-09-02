// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMBS_BUILD_SERVER_INCLUDED
#define CMBS_BUILD_SERVER_INCLUDED
#include <string>

namespace cmbs {

void StartBuildServer(int port, const std::string& version, int timeoutSecs, bool log);
void StopBuildServer();
bool BuildServerTimeOut();
bool BuildServerStopRequested();
std::string CmbsLogFilePath();

} // namespace cmbs

#endif // CMBS_BUILD_SERVER_INCLUDED
