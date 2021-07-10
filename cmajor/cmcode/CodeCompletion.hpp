// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_CODE_COMPLETION_INCLUDED
#define CMCODE_CODE_COMPLETION_INCLUDED
#include <string>
#include <vector>

namespace cmcode {

void StartCodeCompletionService(int pid);
void StopCodeCompletionService(bool log);
void LoadEditModule(const std::string& projectFilePath, const std::string& backend, const std::string& config);
void ResetEditModuleCache();
void ParseSource(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, std::u32string&& sourceCode);
void GetCCList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, const std::u32string& ccText, const std::vector<int>& ruleContext,
    const std::u32string& cursorLine);
void GetParamHelpList(const std::string& projectFilePath, const std::string& backend, const std::string& config, const std::string& sourceFilePath, int symbolIndex);

} // namespace cmcode

#endif // CMCODE_CODE_COMPLETION_INCLUDED
