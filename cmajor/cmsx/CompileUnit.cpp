// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsx/CompileUnit.hpp>
#include <fstream>

namespace cmsx {

CompileUnit::CompileUnit(const std::string& assemblyFilePath_) : assemblyFilePath(assemblyFilePath_)
{
}

void CompileUnit::Write()
{
    std::ofstream assembly(assemblyFilePath);
}

} // namespace cmsx
