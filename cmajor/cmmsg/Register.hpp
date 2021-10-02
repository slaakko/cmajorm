// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MSG_REGISTER_INCLUDED
#define CMAJOR_MSG_REGISTER_INCLUDED
#include <cmajor/cmmsg/MsgApi.hpp>

namespace cmmsg {

const int stopBuildRequestClassId = 1;
const int stopBuildReplyClassId = 2;
const int keepAliveBuildRequestClassId = 3;
const int keepAliveBuildReplyClassId = 4;
const int buildRequestClassId = 5;
const int compileErrorClassId = 6;
const int buildReplyClassId = 7;
const int logBuildMessageRequestClassId = 8;
const int logBuildMessageReplyClassId = 9;
const int buildProgressMessageClassId = 10;
const int genericBuildErrorReplyClassId = 11;
const int cacheModuleRequestClassId = 12;
const int cacheModuleReplyClassId = 13;
const int definitionSourceLocationClassId = 14;
const int getDefinitionRequestClassId = 15;
const int getDefinitionReplyClassId = 16;

const int locationClassId = 20;
const int typeClassId = 21;
const int resultClassId = 22;
const int childResultClassId = 23;
const int targetStateClassId = 24;
const int sourceLocClassId = 25;
const int breakpointInfoClassId = 26;
const int startDebugRequestClassId = 27;
const int startDebugReplyClassId = 28;
const int stopDebugRequestClassId = 29;
const int stopDebugReplyClassId = 30;
const int genericDebugErrorReplyClassId = 31;
const int continueRequestClassId = 32;
const int continueReplyClassId = 33;
const int nextRequestClassId = 34;
const int nextReplyClassId = 35;
const int stepRequestClassId = 36;
const int stepReplyClassId = 37;
const int finishRequestClassId = 38;
const int finishReplyClassId = 39;
const int untilRequestClassId = 40;
const int untilReplyClassId = 41;
const int breakRequestClassId = 42;
const int breakReplyClassId = 43;
const int deleteRequestClassId = 44;
const int deleteReplyClassId = 45;
const int depthRequestClassId = 46;
const int depthReplyClassId = 47;
const int framesRequestClassId = 48;
const int framesReplyClassId = 49;
const int evaluateRequestClassId = 50;
const int evaluateReplyClassId = 51;
const int countRequestClassId = 52;
const int countReplyClassId = 53;
const int evaluateChildRequestClassId = 54;
const int evaluateChildReplyClassId = 55;
const int targetRunningRequestClassId = 56;
const int targetRunningReplyClassId = 57;
const int targetInputRequestClassId = 58;
const int targetInputReplyClassId = 59;
const int targetOutputRequestClassId = 60;
const int targetOutputReplyClassId = 61;
const int logDebugMessageRequestClassId = 62;
const int logDebugMessageReplyClassId = 63;
const int killRequestClassId = 64;
const int killReplyClassId = 65;

const int codeCompletionRequestClassId = 80;
const int codeCompletionReplyClassId = 81;
const int loadEditModuleRequestClassId = 82;
const int loadEditModuleReplyClassId = 83;
const int resetEditModuleCacheRequestClassId = 84;
const int resetEditModuleCacheReplyClassId = 85;
const int parseSourceRequestClassId = 86;
const int parseSourceReplyClassId = 87;
const int getCCListRequestClassId = 88;
const int getCCListReplyClassId = 89;
const int getParamHelpListRequestClassId = 90;
const int getParamHelpListReplyClassId = 91;
const int stopCCRequestClassId = 92;
const int stopCCReplyClassId = 93;
const int keepAliveCCRequestClassId = 94;
const int keepAliveCCReplyClassId = 95;

MSG_API void Register();

} // namespace cmmsg;

#endif // CMAJOR_MSG_REGISTER_INCLUDED
