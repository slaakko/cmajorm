// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/Themes.hpp>
#include <system-x/db/ToolBar.hpp>
#include <wing/Theme.hpp>

namespace cmsx::db {

using namespace cmajor::wing;

void InitThemes()
{
    LoadThemes();
    SetTheme("light");
}

} // namespace cmsx::db
