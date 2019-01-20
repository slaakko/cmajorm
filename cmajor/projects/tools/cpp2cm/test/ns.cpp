namespace alpha::beta
{
    int x;
}

namespace alpha::gamma
{
    using namespace alpha::beta;
    void main()
    {
        x = 1;
    }
}

namespace
{
    bool y = true;
}

void foo()
{
    y = false;
}
