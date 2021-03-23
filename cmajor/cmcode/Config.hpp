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
void LoadConfiguration();
void SaveConfiguration();
const BuildSettings& GetBuildSettings();
WindowSettings& GetWindowSettings();
void ConfigInit();
void ConfigDone();

} // namespace cmcode

#endif // CMCODE_CONFIG_INCLUDED
