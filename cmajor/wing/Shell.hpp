// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_SHELL_VIEW_INCLUDED
#define CMAJOR_WING_SHELL_VIEW_INCLUDED
#include <wing/WingApi.hpp>
#include <string>

namespace cmajor { namespace wing {

WING_API void ShellInit();
WING_API void ShellDone();
WING_API std::string GetProgramFilesDirectoryPath();
WING_API std::string GetStartMenuProgramsFolderPath();
WING_API std::string GetDesktopFolderPath();
WING_API void GetShellLinkData(const std::string& linkFilePath, std::string& path, std::string& arguments, std::string& workingDirectory, 
    std::string& description, std::string& iconPath, int& iconIndex);
WING_API void CreateShellLink(const std::string& linkFilePath, const std::string& path, const std::string& arguments, const std::string& workingDirectory, 
    const std::string& description, const std::string& iconPath, int iconIndex);

} } // cmajor::wing

#endif // CMAJOR_WING_SHELL_VIEW_INCLUDED
