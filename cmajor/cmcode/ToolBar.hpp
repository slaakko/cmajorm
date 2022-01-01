// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_TOOL_BAR_INCLUDED
#define CMCODE_TOOL_BAR_INCLUDED
#include <cmajor/wing/ToolBar.hpp>

namespace cmcode {

using namespace cmajor::wing;

Color DefaultToolBarBorderColor();
Color DefaultToolBarTextButtonColor();
Color DefaultToolBarToolTipColor();
int DefaultToolBarToolButtonHeight();
Padding DefaultToolBarPadding();
Padding DefaultToolBarToolButtonPadding();

ToolBar* MakeToolBar();

} // namespace cmcode

#endif // CMCODE_TOOL_BAR_INCLUDED
