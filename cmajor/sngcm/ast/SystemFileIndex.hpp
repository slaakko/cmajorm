// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_SYSTEM_FILE_INDEX_INCLUDED
#define SNGCM_AST_SYSTEM_FILE_INDEX_INCLUDED
#include <soulng/util/Error.hpp>
#include <map>
#include <memory>
#include <stdint.h>

namespace sngcm { namespace ast { 

const uint32_t firstSystemFileIndex = 1000000;

class SystemFileIndex
{
public:
    static void Init();
    static SystemFileIndex& Instance() { Assert(instance, "system file index not initialized");  return *instance; }
    uint32_t RegisterSystemSourceFile(const std::string& systemSourceFilePath);
    std::string GetSystemSourceFilePath(uint32_t systemFileIndex) const;
    void Write(const std::string& systemFileIndexFilePath);
    void Read(const std::string& systemFileIndexFilePath);
private:
    static std::unique_ptr<SystemFileIndex> instance;
    uint32_t nextSystemFileIndex;
    std::string cmajorRootDir;
    std::map<uint32_t, std::string> systemFileMap;
    SystemFileIndex();
};

void SystemFileIndexInit();

} } // namespace sngcm::ast

#endif
