// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parser/FileRegistry.hpp>
#include <cmajor/ast/SystemFileIndex.hpp>
#include <mutex>

namespace cmajor { namespace parser {

using namespace cmajor::ast;

std::mutex mtx;

std::unique_ptr<FileRegistry> FileRegistry::instance;

void FileRegistry::Init()
{
    instance.reset(new FileRegistry());
}

void FileRegistry::Clear()
{
    fileMap.clear();
}

void FileRegistry::SetNextUserFileIndex(uint32_t nextUserFileindex_)
{
    nextUserFileIndex = nextUserFileindex_;
}

FileRegistry::FileRegistry() : obtainSystemFileIndeces(false), nextUserFileIndex(1)
{
}

void FileRegistry::PushObtainSystemFileIndeces()
{
    obtainsSystemFileIndecesStack.push(obtainSystemFileIndeces);
    obtainSystemFileIndeces = true;
}

void FileRegistry::PopObtainSystemFileIndeces()
{
    obtainSystemFileIndeces = obtainsSystemFileIndecesStack.top();
    obtainsSystemFileIndecesStack.pop();
}

uint32_t FileRegistry::RegisterNewFile(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (obtainSystemFileIndeces)
    {
        uint32_t fileIndex = SystemFileIndex::Instance().RegisterSystemSourceFile(filePath);
        fileMap[fileIndex] = std::make_pair(true, filePath);
        return fileIndex;
    }
    else
    {
        uint32_t fileIndex = nextUserFileIndex++;
        fileMap[fileIndex] = std::make_pair(true, filePath);
        return fileIndex;
    }
}

void FileRegistry::RegisterExistingFile(uint32_t fileIndex, const std::string& filePath)
{
    fileMap[fileIndex] = std::make_pair(false, filePath);
}

std::vector<std::pair<uint32_t, std::string>> FileRegistry::GetFileMap()
{
    std::vector<std::pair<uint32_t, std::string>> result;
    for (const std::pair<uint32_t, std::pair<bool, std::string>>& p : fileMap)
    {
        if (p.second.first)
        {
            result.push_back(std::make_pair(p.first, p.second.second));
        }
    }
    return result;
}

std::string FileRegistry::GetFilePath(uint32_t filePathIndex)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (filePathIndex >= firstSystemFileIndex)
    {
        return SystemFileIndex::Instance().GetSystemSourceFilePath(filePathIndex);
    }
    else
    {
        auto it = fileMap.find(filePathIndex);
        if (it != fileMap.cend())
        {
            return it->second.second;
        }
        return std::string();
    }
}

} } // namespace cmajor::parser
