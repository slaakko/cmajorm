// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <stdexcept>

bool CheckCmajorRootEnv()
{
    try
    {
        soulng::unicode::CmajorRoot();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return false;
    }
    return true;
}

int main()
{
    if (!CheckCmajorRootEnv()) return 1;
    soulng::util::Init();
    try
    {
        soulng::unicode::CharacterTable::Instance().WriteDeflate();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    soulng::util::Done();
}