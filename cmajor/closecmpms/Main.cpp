// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/InitDone.hpp>
#include <cmajor/cmpm/Register.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/serialization/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>

void Init()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    sngxml::xmlser::Init();
    cmajor::service::Init();
    cmpm::Register();
}

int main()
{
    try
    {
        Init();
        cmajor::service::StopPortMapServer();
    }
    catch (...)
    {
    }
    return 0;
}
