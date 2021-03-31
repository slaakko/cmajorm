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

namespace cmajor { namespace service {

void Init()
{
    InitServiceMessage();
    InitRequest();
    InitRequestDispatcher();
    InitBuildService();
    InitDebugService();
}

void Done()
{
    DoneDebugService();
    DoneBuildService();
    DoneRequestDispatcher();
    DoneRequest();
    DoneServiceMessage();
}

} } // namespace cmajor::service
