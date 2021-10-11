#include <cmajor/cmpm/InitDone.hpp>
#include <cmajor/cmpm/PortMapServer.hpp>
#include <cmajor/cmpm/Register.hpp>
#include <sngxml/serialization/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <string>
#include <iostream>
#include <stdexcept>

using namespace soulng::util;

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    sngxml::xmlser::Init();
    cmpm::Register();
    cmajor::cmpm::Init();
}

int main(int argc, const char** argv)
{
    try
    { 
        InitApplication();
        cmajor::cmpm::RunPortMapServer();
    }
    catch (const std::exception& ex)
    {
        std::cout << "port-map-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
