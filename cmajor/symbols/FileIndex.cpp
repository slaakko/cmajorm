// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/FileIndex.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/cmdebug/DebugInfoIo.hpp>
#include <soulng/util/BinaryWriter.hpp>

namespace cmajor { namespace symbols {

void FileIndex::AddFile(int32_t fileIndex, Module* module)
{
    if (fileIndex == -1) return;
    std::lock_guard<std::mutex> lock(mtx);
    if (fileMap.find(fileIndex) == fileMap.cend())
    {
        fileMap[fileIndex] = module->GetFilePath(fileIndex);
    }
}

void FileIndex::Write(BinaryWriter& writer)
{
    cmajor::debug::WriteNumberOfFileIndexRecords(writer, fileMap.size());
    for (const auto& p : fileMap)
    {
        cmajor::debug::WriteFileIndexRecord(writer, p.first, p.second);
    }
}

} } // namespace cmajor::symbols
