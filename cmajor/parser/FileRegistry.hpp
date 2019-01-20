// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSER_FILE_REGISTRY_INCLUDED
#define CMAJOR_PARSER_FILE_REGISTRY_INCLUDED
#include <cmajor/util/Error.hpp>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <map>

namespace cmajor { namespace parser {

class FileRegistry
{
public:
    static void Init();
    static FileRegistry& Instance() { Assert(instance, "file registry not initialized"); return *instance; }
    void Clear();
    void SetNextUserFileIndex(uint32_t nextUserFileindex_);
    uint32_t GetNextUserFileIndex() { return nextUserFileIndex; }
    uint32_t RegisterNewFile(const std::string& filePath);
    void RegisterExistingFile(uint32_t fileIndex, const std::string& filePath);
    std::string GetFilePath(uint32_t filePathIndex);
    std::vector<std::pair<uint32_t, std::string>> GetFileMap();
    void PushObtainSystemFileIndeces();
    void PopObtainSystemFileIndeces();
private:
    static std::unique_ptr<FileRegistry> instance;
    std::map<uint32_t, std::pair<bool, std::string>> fileMap;
    FileRegistry();
    bool obtainSystemFileIndeces;
    std::stack<bool> obtainsSystemFileIndecesStack;
    uint32_t nextUserFileIndex;
};

} } // namespace cmajor::parser

#endif // CMAJOR_PARSER_FILE_REGISTRY_INCLUDED
