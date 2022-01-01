// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/Trace.hpp>

void CreateTraceFunctions()
{
    soulng::util::GetTraceFunctionId("main");
    soulng::util::GetTraceFunctionId("GetMessageKind(JsonValue*)");
    soulng::util::GetTraceFunctionId("ReadMessage(TcpSocket&)");
    soulng::util::GetTraceFunctionId("ProcessBuildReply(const BuildReply&)");
    soulng::util::GetTraceFunctionId("RunRequest(const std::string&, int)");
    soulng::util::GetTraceFunctionId("BackendSelector.BackendSelector(cmajor::symbols::BackEnd)");
    soulng::util::GetTraceFunctionId("BackendSelector.~BackendSelector()");
    soulng::util::GetTraceFunctionId("CmbsLogFilePath()");
    soulng::util::GetTraceFunctionId("RequestGuard.RequestGuard(BuildServer*)");
    soulng::util::GetTraceFunctionId("RequestGuard.~RequestGuard()");
    soulng::util::GetTraceFunctionId("BuildServer.BuildServer(bool)");
    soulng::util::GetTraceFunctionId("BuildServer.~BuildServer()");
    soulng::util::GetTraceFunctionId("BuildServer.Run()");
    soulng::util::GetTraceFunctionId("BuildServer.WriteGenericErrorReply(const std::string&)");
    soulng::util::GetTraceFunctionId("BuildServer.ProcessBuildRequest(const BuildRequest&, LogFileWriter*)");
    soulng::util::GetTraceFunctionId("BuildServer.ProcessCacheModuleRequest(const CacheModuleRequest&)");
    soulng::util::GetTraceFunctionId("BuildServer.ProcessGetDefinitionRequest(const GetDefinitionRequest&)");
    soulng::util::GetTraceFunctionId("BuildServer.ProcessCppBackendRequest(const BuildRequest&, BuildReply&, LogFileWriter*)");
    soulng::util::GetTraceFunctionId("BuildServer.ProcessLlvmBackendRequest(const BuildRequest&, BuildReply&, LogFileWriter*)");
    soulng::util::GetTraceFunctionId("BuildServer.BuildSolution(const std::string&, std::vector<std::unique_ptr<cmajor::symbols::Module>>&, BuildReply&, LogFileWriter*)");
    soulng::util::GetTraceFunctionId("BuildServer.BuildProject(const std::string&, std::unique_ptr<cmajor::symbols::Module>&, std::set<std::string>&, BuildReply&, LogFileWriter*)");
    soulng::util::GetTraceFunctionId("BuildServer.SetRunException(const std::exception_ptr&)");
    soulng::util::GetTraceFunctionId("BuildServer.SetLogException(const std::exception_ptr&)");
    soulng::util::GetTraceFunctionId("BuildServer.SetLogExceptionToReply(BuildReply&)");
    soulng::util::GetTraceFunctionId("BuildServer.StartLogging()");
    soulng::util::GetTraceFunctionId("BuildServer.EndLogging()");
    soulng::util::GetTraceFunctionId("BuildServer.RunLog()");
    soulng::util::GetTraceFunctionId("RunLogThread(BuildServer*)");
    soulng::util::GetTraceFunctionId("BuildServer.StartLogThread()");
    soulng::util::GetTraceFunctionId("BuildServer.StopLogThread()");
    soulng::util::GetTraceFunctionId("BuildServer.GetMessageKind(JsonValue*)");
    soulng::util::GetTraceFunctionId("RunServer(BuildServer*)");
    soulng::util::GetTraceFunctionId("BuildServer.Start(int, const std::string&, std::condition_variable*, bool*)");
    soulng::util::GetTraceFunctionId("BuildServer.Stop()");
    soulng::util::GetTraceFunctionId("BuildServer.SetRequestInProgress()");
    soulng::util::GetTraceFunctionId("BuildServer.ResetRequestInProgress()");
    soulng::util::GetTraceFunctionId("StartBuildServer(int, const std::string&, bool, std::condition_variable*, bool*)");
    soulng::util::GetTraceFunctionId("StopBuildServer()");
    soulng::util::GetTraceFunctionId("BuildServerStopRequested()");
    soulng::util::GetTraceFunctionId("CmajorRootDir()");
    soulng::util::GetTraceFunctionId("CmajorLogDir()");
    soulng::util::GetTraceFunctionId("LogFilePath()");
    soulng::util::GetTraceFunctionId("KeepAliveServer.KeepAliveServer()");
    soulng::util::GetTraceFunctionId("KeepAliveServer.GetMessage(JsonValue*)");
    soulng::util::GetTraceFunctionId("KeepAliveServer.ProcessKeepAliveRequest(const KeepAliveRequest&)");
    soulng::util::GetTraceFunctionId("KeepAliveServer.Run()");
    soulng::util::GetTraceFunctionId("KeepAliveServer.Timeout()");
    soulng::util::GetTraceFunctionId("RunServer(KeepAliveServer*)");
    soulng::util::GetTraceFunctionId("KeepAliveServer.Start(int, std::condition_variable*, bool*)");
    soulng::util::GetTraceFunctionId("KeepAliveServer.Stop()");
    soulng::util::GetTraceFunctionId("StartKeepAliveServer(int, std::condition_variable*, bool*)");
    soulng::util::GetTraceFunctionId("StopKeepAliveServer()");
    soulng::util::GetTraceFunctionId("Timeout()");
    soulng::util::WriteTraceTable();
}
