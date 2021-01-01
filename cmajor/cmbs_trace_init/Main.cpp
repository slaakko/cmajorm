#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/InitDone.hpp>

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
    InitDone initDone;
    CreateTraceFunctions();
    return 0;
}
