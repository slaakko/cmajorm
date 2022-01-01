// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/InitDone.hpp>
#include <cmajor/cmview/Bitmaps.hpp>

namespace cmajor { namespace view {

void Init()
{
    InitBitmaps();
}

void Done()
{
    DoneBitmaps();
}

} } // namespace cmajor::view
