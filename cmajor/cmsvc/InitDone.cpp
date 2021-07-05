// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/InitDone.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/Request.hpp>
#include <cmajor/cmsvc/RequestDispatcher.hpp>
#include <cmajor/cmsvc/BuildService.hpp>
#include <cmajor/cmsvc/DebugService.hpp>
#include <cmajor/cmsvc/RunService.hpp>
#include <cmajor/cmsvc/CodeCompletionService.hpp>
#include <cmajor/cmdebug/CmdbSession.hpp>

namespace cmajor { namespace service {

void Init()
{
    InitServiceMessage();
    InitRequest();
    InitRequestDispatcher();
    InitBuildService();
    InitDebugService();
    InitRunService();
    InitCodeCompletionService();
    cmajor::debug::CmdbSessionInit();
}

void Done()
{
    cmajor::debug::CmdbSessionDone();
    DoneCodeCompletionService();
    DoneRunService();
    DoneDebugService();
    DoneBuildService();
    DoneRequestDispatcher();
    DoneRequest();
    DoneServiceMessage();
}

} } // namespace cmajor::service
