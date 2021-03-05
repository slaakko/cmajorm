// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_DIALOG_INCLUDED
#define CMAJOR_WING_DIALOG_INCLUDED
#include <cmajor/wing/WingApi.hpp>
#include <string>
#include <vector>
#include <Windows.h>

namespace cmajor { namespace wing {

// OFN_PATHMUSTEXIST, OFN_FILEMUSTEXIST

WING_API bool OpenFileName(HWND windowHandle, const std::vector<std::pair<std::string, std::string>>& descriptionFilterPairs, const std::string& initialDirectory,
    const std::string& defaultFilePath, const std::string& defaultExtension,
    uint32_t flags, std::string& firstPath, std::string& currentDirectory, std::vector<std::string>& fileNames);

} } // cmajor::wing

#endif // CMAJOR_WING_DIALOG_INCLUDED
