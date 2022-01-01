// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmsg/Register.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <cmajor/cmmsg/CodeCompletionServerMessage.hpp>

namespace cmmsg {

void Register()
{
    StopBuildRequest::Register(stopBuildRequestClassId);
    StopBuildReply::Register(stopBuildReplyClassId);
    KeepAliveBuildRequest::Register(keepAliveBuildRequestClassId);
    KeepAliveBuildReply::Register(keepAliveBuildReplyClassId);
    BuildRequest::Register(buildRequestClassId);
    CompileError::Register(compileErrorClassId);
    BuildReply::Register(buildReplyClassId);
    LogBuildMessageRequest::Register(logBuildMessageRequestClassId);
    LogBuildMessageReply::Register(logBuildMessageReplyClassId);
    BuildProgressMessage::Register(buildProgressMessageClassId);
    GenericBuildErrorReply::Register(genericBuildErrorReplyClassId);
    CacheModuleRequest::Register(cacheModuleRequestClassId);
    CacheModuleReply::Register(cacheModuleReplyClassId);
    DefinitionSourceLocation::Register(definitionSourceLocationClassId);
    GetDefinitionRequest::Register(getDefinitionRequestClassId);
    GetDefinitionReply::Register(getDefinitionReplyClassId);

    Location::Register(locationClassId);
    Type::Register(typeClassId);
    Result::Register(resultClassId);
    ChildResult::Register(childResultClassId);
    TargetState::Register(targetStateClassId);
    SourceLoc::Register(sourceLocClassId);
    BreakpointInfo::Register(breakpointInfoClassId);
    StartDebugRequest::Register(startDebugRequestClassId);
    StartDebugReply::Register(startDebugReplyClassId);
    StopDebugRequest::Register(stopDebugRequestClassId);
    StopDebugReply::Register(stopDebugReplyClassId);
    GenericDebugErrorReply::Register(genericDebugErrorReplyClassId);
    ContinueRequest::Register(continueRequestClassId);
    ContinueReply::Register(continueReplyClassId);
    NextRequest::Register(nextRequestClassId);
    NextReply::Register(nextReplyClassId);
    StepRequest::Register(stepRequestClassId);
    StepReply::Register(stepReplyClassId);
    FinishRequest::Register(finishRequestClassId);
    FinishReply::Register(finishReplyClassId);
    UntilRequest::Register(untilRequestClassId);
    UntilReply::Register(untilReplyClassId);
    BreakRequest::Register(breakRequestClassId);
    BreakReply::Register(breakReplyClassId);
    DeleteRequest::Register(deleteRequestClassId);
    DeleteReply::Register(deleteReplyClassId);
    DepthRequest::Register(depthRequestClassId);
    DepthReply::Register(depthReplyClassId);
    FramesRequest::Register(framesRequestClassId);
    FramesReply::Register(framesReplyClassId);
    EvaluateRequest::Register(evaluateRequestClassId);
    EvaluateReply::Register(evaluateReplyClassId);
    CountRequest::Register(countRequestClassId);
    CountReply::Register(countReplyClassId);
    EvaluateChildRequest::Register(evaluateChildRequestClassId);
    EvaluateChildReply::Register(evaluateChildReplyClassId);
    TargetRunningRequest::Register(targetRunningRequestClassId);
    TargetRunningReply::Register(targetRunningReplyClassId);
    TargetInputRequest::Register(targetInputRequestClassId);
    TargetInputReply::Register(targetInputReplyClassId);
    TargetOutputRequest::Register(targetOutputRequestClassId);
    TargetOutputReply::Register(targetOutputReplyClassId);
    LogDebugMessageRequest::Register(logDebugMessageRequestClassId);
    LogDebugMessageReply::Register(logDebugMessageReplyClassId);
    KillRequest::Register(killRequestClassId);
    KillReply::Register(killReplyClassId);

    CodeCompletionRequest::Register(codeCompletionRequestClassId);
    CodeCompletionReply::Register(codeCompletionReplyClassId);
    LoadEditModuleRequest::Register(loadEditModuleRequestClassId);
    LoadEditModuleReply::Register(loadEditModuleReplyClassId);
    ResetEditModuleCacheRequest::Register(resetEditModuleCacheRequestClassId);
    ResetEditModuleCacheReply::Register(resetEditModuleCacheReplyClassId);
    ParseSourceRequest::Register(parseSourceRequestClassId);
    ParseSourceReply::Register(parseSourceReplyClassId);
    GetCCListRequest::Register(getCCListRequestClassId);
    GetCCListReply::Register(getCCListReplyClassId);
    GetParamHelpListRequest::Register(getParamHelpListRequestClassId);
    GetParamHelpListReply::Register(getParamHelpListReplyClassId);
    StopCCRequest::Register(stopCCRequestClassId);
    StopCCReply::Register(stopCCReplyClassId);
    KeepAliveCCRequest::Register(keepAliveCCRequestClassId);
    KeepAliveCCReply::Register(keepAliveCCReplyClassId);
}

} // namespace cmmsg