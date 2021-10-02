// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/InitDone.hpp>
#include <cmajor/cmpm/Register.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/serialization/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        sngxml::xmlser::Init();
        cmajor::service::Init();
        cmpm::Register();
    }
    ~InitDone()
    {
        cmajor::service::Done();
        sngxml::xmlser::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

int main()
{
    try
    {
        InitDone initDone;
        cmajor::service::StopPortMapServer();
    }
    catch (...)
    {
    }
    return 0;
}
