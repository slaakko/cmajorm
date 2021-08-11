// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <stdexcept>

int main()
{
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