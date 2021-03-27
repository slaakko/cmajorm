// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/Editor.hpp>

namespace cmajor { namespace view {

Editor::Editor(ControlCreateParams& createParams) : ContainerControl(createParams)
{
}

bool Editor::IsCCDirty() const
{
    return false;
}

void Editor::ResetCCDirty()
{
}

} } // namespace cmajor::view
