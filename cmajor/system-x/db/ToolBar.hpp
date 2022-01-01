// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_TOOL_BAR_INCLUDED
#define CMSX_DB_TOOL_BAR_INCLUDED
#include <wing/ToolBar.hpp>

namespace cmsx::db {

using namespace cmajor::wing;

Color DefaultToolBarBorderColor();
Color DefaultToolBarTextButtonColor();
Color DefaultToolBarToolTipColor();
int DefaultToolBarToolButtonHeight();
Padding DefaultToolBarPadding();
Padding DefaultToolBarToolButtonPadding();

ToolBar* MakeToolBar();

} // namespace cmsx::db

#endif // CMSX_DB_TOOL_BAR_INCLUDED
