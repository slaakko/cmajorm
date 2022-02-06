// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Archive.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <soulng/util/Log.hpp>
#include <boost/filesystem.hpp>

namespace cmsx::object {

using namespace soulng::util;

void CreateArchive(int logStreamId, const std::string& archiveFilePath, const std::vector<std::string>& objectFilePaths, bool verbose)
{
    std::unique_ptr<ArchiveFile> archiveFile(new ArchiveFile(archiveFilePath));
    archiveFile->CreateSections();
    int n = objectFilePaths.size();
    for (int i = 0; i < n; ++i)
    {
        if (verbose)
        {
            LogMessage(logStreamId, "> " + objectFilePaths[i]);
        }
        if (boost::filesystem::exists(objectFilePaths[i]))
        {
            std::unique_ptr<BinaryFile> binaryFile(ReadBinaryFile(objectFilePaths[i]));
            if (binaryFile->Kind() == BinaryFileKind::objectFile)
            {
                binaryFile->SetParent(archiveFile.get());
                archiveFile->AddObjectFile(static_cast<ObjectFile*>(binaryFile.release()));
            }
            else
            {
                throw std::runtime_error("object file expected: " + objectFilePaths[i]);
            }
        }
    }
    archiveFile->WriteFile();
    if (verbose)
    {
        LogMessage(logStreamId, "==> " + archiveFilePath);
    }
}

} // namespace cmsx::object
