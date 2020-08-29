// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#ifdef _WIN32
#include <cmajor/cmres/InitDone.hpp>
#endif
#include <sngxml/xpath/InitDone.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
        sngxml::xpath::Init();
#ifdef _WIN32
        cmajor::resources::Init();
#endif
    }
    ~InitDone()
    {
#ifdef _WIN32
        cmajor::resources::Done();
#endif
        sngxml::xpath::Done();
        cmajor::symbols::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

int main(int argc, const char** argv)
{
    try
    {

    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
