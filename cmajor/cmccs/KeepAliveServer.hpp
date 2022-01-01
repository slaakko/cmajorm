// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCCS_KEEP_ALIVE_SERVER_INCLUDED
#define CMCCS_KEEP_ALIVE_SERVER_INCLUDED
#include <condition_variable>

namespace cmccs {

const int keepAliveSecs = 60;
const int timeoutSecs = 120;
const int defaultKeepAliveServerPort = 54328;

std::string CmajorRootDir();
std::string CmajorLogDir();
std::string LogFilePath();

void StartKeepAliveServer(int keepAliveServerPort, std::condition_variable* exitVar, bool* exiting);
void StopKeepAliveServer();
bool Timeout();

} // namespace cmccs

#endif // CMCCS_KEEP_ALIVE_SERVER_INCLUDED
