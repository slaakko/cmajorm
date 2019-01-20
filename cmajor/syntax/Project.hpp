// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYNTAX_PROJECT_INCLUDED
#define CMAJOR_SYNTAX_PROJECT_INCLUDED
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <memory>

namespace cmajor { namespace syntax {

class Project
{
public:
    Project(const std::string& name_, const std::string& filePath_);
    const std::string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    std::string BasePath() const;
    void AddSourceFile(const std::string& sourceFilePath);
    void AddReferenceFile(const std::string& referenceFilePath);
    const std::vector<std::string>& ReferenceFiles() const { return referenceFiles; }
    const std::vector<std::string>& SourceFiles() const { return sourceFiles; }
private:
    std::string name;
    std::string filePath;
    std::vector<std::string> sourceFiles;
    std::vector<std::string> referenceFiles;
    boost::filesystem::path base;
};

} } // namespace cmajor::syntax

#endif // CMAJOR_SYNTAX_PROJECT_INCLUDED
