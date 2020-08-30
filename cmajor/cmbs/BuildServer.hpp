// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMBS_BUILD_SERVER_INCLUDED
#define CMBS_BUILD_SERVER_INCLUDED
#include <string>

namespace cmbs {

void StartBuildServer(int port, bool verbose, const std::string& version);
void StopBuildServer();

} // namespace cmbs

#endif // CMBS_BUILD_SERVER_INCLUDED
