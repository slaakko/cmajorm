// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpmv/Themes.hpp>
#include <wing/Theme.hpp>

namespace cmajor { namespace pmv {

using namespace cmajor::wing;

void InitThemes()
{
    LoadThemes();
    SetTheme("light");
}

} } // namespace cmajor::pmv

