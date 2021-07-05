// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_CODE_COMPLETION_INCLUDED
#define CMCODE_CODE_COMPLETION_INCLUDED
#include <string>

namespace cmcode {

void StartCodeCompletionService(int pid);
void StopCodeCompletionService(bool log);
void LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config);

} // namespace cmcode

#endif // CMCODE_CODE_COMPLETION_INCLUDED
