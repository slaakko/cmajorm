// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/InitDone.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/Request.hpp>
#include <cmajor/cmsvc/RequestDispatcher.hpp>

namespace cmajor { namespace service {

void Init()
{
    InitServiceMessage();
    InitRequest();
    InitRequestDispatcher();
}

void Done()
{
    DoneRequestDispatcher();
    DoneRequest();
    DoneServiceMessage();
}

} } // namespace cmajor::service
