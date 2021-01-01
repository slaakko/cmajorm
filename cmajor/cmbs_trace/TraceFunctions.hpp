// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMBS_TRACE_TRACE_FUNCTIONS_INCLUDED
#define CMBS_TRACE_TRACE_FUNCTIONS_INCLUDED
#include <cmajor/cmbs_trace/Api.hpp>
#include <stdint.h>

const int32_t main_f = 0;
const int32_t GetMessageKind_f = 1;
const int32_t ReadMessage_f = 2;
const int32_t ProcessBuildReply_f = 3;
const int32_t RunRequest_f = 4;
const int32_t BackendSelector_BackendSelector = 5;
const int32_t BackendSelector_BackendSelector_dtor = 6;
const int32_t CmbsLogFilePath_f = 7;
const int32_t RequestGuard_RequestGuard = 8;
const int32_t RequestGuard_RequestGuard_dtor = 9;
const int32_t BuildServer_BuildServer = 10;
const int32_t BuildServer_BuildServer_dtor = 11;
const int32_t BuildServer_Run = 12;
const int32_t BuildServer_WriteGenericErrorReply = 13;
const int32_t BuildServer_ProcessBuildRequest = 14;
const int32_t BuildServer_ProcessCacheModuleRequest = 15;
const int32_t BuildServer_ProcessGetDefinitionRequest = 16;
const int32_t BuildServer_ProcessCppBackendRequest = 17;
const int32_t BuildServer_ProcessLlvmBackendRequest = 18;
const int32_t BuildServer_BuildSolution = 19;
const int32_t BuildServer_BuildProject = 20;
const int32_t BuildServer_SetRunException = 21;
const int32_t BuildServer_SetLogException = 22;
const int32_t BuildServer_SetLogExceptionToReply = 23;
const int32_t BuildServer_StartLogging = 24;
const int32_t BuildServer_EndLogging = 25;
const int32_t BuildServer_RunLog = 26;
const int32_t RunLogThread_f = 27;
const int32_t BuildServer_StartLogThread = 28;
const int32_t BuildServer_StopLogThread = 29;
const int32_t BuildServer_GetMessageKind = 30;
const int32_t RunServer_BuildServer_f = 31;
const int32_t BuildServer_Start = 32;
const int32_t BuildServer_Stop = 33;
const int32_t BuildServer_SetRequestInProgress = 34;
const int32_t BuildServer_ResetRequestInProgress = 35;
const int32_t StartBuildServer_f = 36;
const int32_t StopBuildServer_f = 37;
const int32_t BuildServerStopRequested_f = 38;
const int32_t CmajorRootDir_f = 39;
const int32_t CmajorLogDir_f = 40;
const int32_t LogFilePath_f = 41;
const int32_t KeepAliveServer_KeepAliveServer = 42;
const int32_t KeepAliveServer_GetMessage = 43;
const int32_t KeepAliveServer_ProcessKeepAliveRequest = 44;
const int32_t KeepAliveServer_Run = 45;
const int32_t KeepAliveServer_Timeout = 46;
const int32_t RunServer_KeepAliveServer_f = 47;
const int32_t KeepAliveServer_Start = 48;
const int32_t KeepAliveServer_Stop = 49;
const int32_t StartKeepAliveServer_f = 50;
const int32_t StopKeepAliveServer_f = 51;
const int32_t Timeout_f = 52;

CMBSTRACE_API void CreateTraceFunctions();

#endif // CMBS_TRACE_TRACE_FUNCTIONS_INCLUDED
