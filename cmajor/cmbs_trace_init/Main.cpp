#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/InitDone.hpp>
#include <iostream>

void Init()
{
    soulng::util::Init();
}

int main()
{
    try
    {
        Init();
        CreateTraceFunctions();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
