// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <string>

namespace cmsx {

class CompileUnit
{
public:
    CompileUnit(const std::string& assemblyFilePath_);
    const std::string& AssemblyFilePath() const { return assemblyFilePath; }
    void Write();
private:
    std::string assemblyFilePath;
};

} // namespace cmsx
