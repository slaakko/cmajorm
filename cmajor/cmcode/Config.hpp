// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_CONFIG_INCLUDED
#define CMCODE_CONFIG_INCLUDED
#include <cmajor/cmcode/Configuration.hpp>

namespace cmcode {

const int configurationSavePeriod = 3000;
const int configurationSaveTimerId = 11;

bool UseDebugServers();
bool ServerLogging();
bool DebugWait();
bool ServerVerbose();
void LoadConfiguration();
void SaveConfiguration();
const Options& GetOptions();
void SetOptions(const Options& options);
const BuildSettings& GetBuildSettings();
void SetBuildSettings(const BuildSettings& buildSettings);
WindowSettings& GetWindowSettings();
const std::vector<RecentSolution>& GetRecentSolutions();
void AddRecentSolution(const std::string& solutionName, const std::string& solutionFilePath);
void RemoveRecentSolution(const std::string& solutionFilePath);
void ConfigInit();
void ConfigDone();

} // namespace cmcode

#endif // CMCODE_CONFIG_INCLUDED
