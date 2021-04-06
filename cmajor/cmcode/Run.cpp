// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Run.hpp>
#include <cmajor/cmsvc/RunServiceRequest.hpp>
#include <soulng/util/Path.hpp>

namespace cmcode {

using namespace cmajor::service;
using namespace soulng::util;

void RunProgram(const std::string& backend, const std::string& config, sngcm::ast::Project* project, const std::string& programArguments)
{
    std::string executableName = Path::Combine(project->OutdirBasePath().generic_string(), "bin");
    if (backend == "llvm")
    {
        executableName = GetFullPath(Path::ChangeExtension(Path::Combine(Path::Combine(executableName, config), Path::GetFileName(project->FilePath())), ".exe"));
    }
    else if (backend == "cpp")
    {
        executableName = GetFullPath(Path::ChangeExtension(Path::Combine(Path::Combine(Path::Combine(Path::Combine(executableName, "cpp"), "gcc"), config), 
            Path::GetFileName(project->FilePath())), ".exe"));
    }
    PutRequest(new StartProgramRequest(executableName, programArguments, "cmcode"));
}

} // namespace cmcode
