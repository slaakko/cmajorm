// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMBS_KEEP_ALIVE_SERVER_INCLUDED
#define CMBS_KEEP_ALIVE_SERVER_INCLUDED
#include <condition_variable>

namespace cmbs {

const int keepAliveSecs = 60;
const int timeoutSecs = 120;
const int defaultKeepAliveServerPort = 54329;

void StartKeepAliveServer(int keepAliveServerPort, std::condition_variable* exitVar, bool* exiting);
void StopKeepAliveServer();
bool Timeout();

} // namespace cmbs

#endif // CMBS_KEEP_ALIVE_SERVER_INCLUDED
