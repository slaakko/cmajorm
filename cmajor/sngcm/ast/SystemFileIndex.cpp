// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/SystemFileIndex.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <mutex>

namespace sngcm { namespace ast {

using namespace soulng::util;

std::unique_ptr<SystemFileIndex> SystemFileIndex::instance;

void SystemFileIndex::Init()
{
    instance.reset(new SystemFileIndex());
}

SystemFileIndex::SystemFileIndex() : nextSystemFileIndex(firstSystemFileIndex), cmajorRootDir(GetFullPath(CmajorRootDir()))
{
}

std::mutex mtx;

uint32_t SystemFileIndex::RegisterSystemSourceFile(const std::string& systemSourceFilePath)
{
    std::lock_guard<std::mutex> lock(mtx);
    uint32_t fileIndex = nextSystemFileIndex++;
    std::string sfp = GetFullPath(systemSourceFilePath);
    if (sfp.find(cmajorRootDir, 0) == 0)
    {
        sfp = sfp.substr(cmajorRootDir.size());
    }
    else
    {
        throw std::runtime_error("CMAJOR_ROOT path prefix '" + cmajorRootDir + "' differs from prefix of system source file path '" + sfp + "'");
    }
    systemFileMap[fileIndex] = sfp;
    return fileIndex;
}

std::string SystemFileIndex::GetSystemSourceFilePath(uint32_t systemFileIndex) const
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = systemFileMap.find(systemFileIndex);
    if (it != systemFileMap.cend())
    {
        std::string sourceFilePath = it->second;
        boost::filesystem::path p(cmajorRootDir);
        p /= sourceFilePath;
        std::string sfp = GetFullPath(p.generic_string());
        return sfp;
    }
    else
    {
        throw std::runtime_error("system file index contains no system source file path for index " + std::to_string(systemFileIndex));
    }
}

void SystemFileIndex::Write(const std::string& systemFileIndexFilePath)
{
    std::lock_guard<std::mutex> lock(mtx);
    BinaryWriter writer(systemFileIndexFilePath);
    writer.Write(nextSystemFileIndex);
    uint32_t n = systemFileMap.size();
    writer.Write(n);
    for (const auto& p : systemFileMap)
    {
        uint32_t index = p.first;
        const std::string& sourceFilePath = p.second;
        writer.Write(index);
        writer.Write(sourceFilePath);
    }
}

void SystemFileIndex::Read(const std::string& systemFileIndexFilePath)
{
    std::lock_guard<std::mutex> lock(mtx);
    nextSystemFileIndex = firstSystemFileIndex;
    systemFileMap.clear();
    BinaryReader reader(systemFileIndexFilePath);
    nextSystemFileIndex = reader.ReadUInt();
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        uint32_t index = reader.ReadUInt();
        std::string sourceFilepath = reader.ReadUtf8String();
        systemFileMap[index] = sourceFilepath;
    }
}

void SystemFileIndexInit()
{
    SystemFileIndex::Init();
}

} } // namespace sngcm::ast
