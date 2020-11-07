#include <cmajor/cmpm/InitDone.hpp>
#include <cmajor/cmpm/PortMapServer.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <string>
#include <iostream>
#include <stdexcept>

using namespace soulng::util;

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::cmpm::Init();
    }
    ~InitDone()
    {
        cmajor::cmpm::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

int main(int argc, const char** argv)
{
    try
    { 
        InitDone initDone;
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
