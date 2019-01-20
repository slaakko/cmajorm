// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_INPUT_INCLUDED
#define CMAJOR_CMDOCLIB_INPUT_INCLUDED
#include <cmajor/dom/Document.hpp>
#include <mutex>

namespace cmajor { namespace cmdoclib {

std::mutex& GetInputMutex();

struct Input
{
    std::string baseDir;
    std::string solutionFilePath;
    std::string docFilePath;
    std::unique_ptr<dom::Document> docs;
    std::string relativeSolutionFilePath;
    std::string targetDirPath;
    std::vector<std::string> libraryDirs;
    std::unordered_map<std::u32string, std::string> libraryPrefixMap;
    std::unordered_map<std::u32string, std::string> grammarMap;
    std::vector<std::unique_ptr<dom::Document>> externalModuleDocs;
};

Input ReadInputXml(const std::string& cmDocFilePath);
void SetInputPtr(Input* inputPtr);
Input* GetInputPtr();
void SetEmptyLibraryPrefix(const std::u32string& moduleName);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_INPUT_INCLUDED
