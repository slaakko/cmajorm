// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Debug.hpp>
#include <cmajor/cmcode/Config.hpp>
#include <cmajor/cmsvc/DebugServiceRequest.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmcode {

using namespace soulng::unicode;

DebugServiceStartParams MakeDebugServiceStartParams(int pid, const std::string& backend, const std::string& config, sngcm::ast::Project* project, const std::string& programArguments)
{
    DebugServiceStartParams startParams;
    return startParams.ProcessName("cmcode").Pid(pid).Backend(backend).Config(config).ProjectFilePath(project->FilePath()).ExecutableName(ToUtf8(project->Name())).ProgramArguments(programArguments).
        DebugServer(UseDebugServers()).Log(ServerLogging()).Wait(DebugWait()).Verbose(ServerVerbose());
}

void StartDebugService(int pid, const std::string& backend, const std::string& config, sngcm::ast::Project* project, const std::string& programArguments, const std::vector<Breakpoint*>& breakpoints)
{
    DebugServiceStartParams startParams = MakeDebugServiceStartParams(pid, backend, config, project, programArguments);
    PutRequest(new StartDebugServiceRequest(startParams, breakpoints));
}

} // namespace cmcode
