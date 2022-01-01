// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_ARCHIVE_INCLUDED
#define CMSX_OBJECT_ARCHIVE_INCLUDED
#include <system-x/object/Api.hpp>
#include <string>
#include <vector>

namespace cmsx::object {

CMSX_OBJECT_API void CreateArchive(int logStreamId, const std::string& archiveFilePath, const std::vector<std::string>& objectFilePaths, bool verbose);

} // namespace cmsx::object

#endif // CMSX_OBJECT_ARCHIVE_INCLUDED
