// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Build.hpp>
#include <cmajor/cmcode/Config.hpp>
#include <cmajor/cmsvc/BuildServiceRequest.hpp>

namespace cmcode {

using namespace cmajor::service;

void StartBuild(const std::string& backend, const std::string& config, const std::string& filePath, BuildRequestKind requestKind)
{
    BuildServiceStartParams serviceStartParams = BuildServiceStartParams().ProcessName("cmcode").DebugServer(UseDebugServers()).Log(ServerLogging()).Wait(DebugWait());
    BuildRequest buildRequest;
    const BuildSettings& buildSettings = GetBuildSettings();
    buildRequest.backend = backend;
    buildRequest.config = config;
    buildRequest.optimizationLevel = "default";
    buildRequest.filePath = filePath;
    buildRequest.numBuildThreads = "default";
    if (buildSettings.numberOfProjectBuildThreads != 0)
    {
        buildRequest.numBuildThreads = std::to_string(buildSettings.numberOfProjectBuildThreads);
    }
    buildRequest.singleThreadedCompile = buildSettings.singleThreadedCompile;
    buildRequest.emitIR = buildSettings.generateIntermediateCodeFiles;
    buildRequest.emitOptIR = buildSettings.generateOptimizedIntermediateCodeFiles;
    buildRequest.toolChain = "default";
    buildRequest.verbose = true;
    if ((requestKind & BuildRequestKind::clean) != BuildRequestKind::none)
    {
        buildRequest.clean = true;
    }
    if ((requestKind & BuildRequestKind::rebuild) != BuildRequestKind::none)
    {
        buildRequest.rebuild = true;
    }
    if ((requestKind & BuildRequestKind::buildDependencies) != BuildRequestKind::none)
    {
        buildRequest.buildAllDependencies = true;
    }
    std::unique_ptr<cmajor::service::StartBuildRequest> startBuildRequest(new cmajor::service::StartBuildRequest(serviceStartParams, buildRequest));
    PutRequest(startBuildRequest.release());
}

void StopBuild()
{
    std::unique_ptr<cmajor::service::StopBuildRequest> stopBuildRequest(new cmajor::service::StopBuildRequest());
    PutRequest(stopBuildRequest.release());
}

void StartGetDefinitionRequest(const GetDefinitionRequest& getDefinitionRequest)
{
    BuildServiceStartParams serviceStartParams = BuildServiceStartParams().ProcessName("cmcode").DebugServer(UseDebugServers()).Log(ServerLogging()).Wait(DebugWait());
    PutRequest(new GotoDefinitionRequest(serviceStartParams, getDefinitionRequest));
}

} // namespace cmcode
