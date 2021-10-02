#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/InitDone.hpp>
#include <iostream>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
    }
    ~InitDone()
    {
        soulng::util::Done();
    }
};


int main()
{
    try
    {
        InitDone initDone;
        CreateTraceFunctions();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
