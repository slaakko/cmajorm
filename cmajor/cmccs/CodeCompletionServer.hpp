// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCCS_CODE_COMPLETION_SERVER_INCLUDED
#define CMCCS_CODE_COMPLETION_SERVER_INCLUDED
#include <condition_variable>
#include <string>

namespace cmccs {

void StartCodeCompletionServer(int port, const std::string& version, std::condition_variable* exitVar, bool* exiting);
void StopCodeCompletionServer();

} // namespace cmccs

#endif // CMCCS_CODE_COMPLETION_SERVER_INCLUDED
