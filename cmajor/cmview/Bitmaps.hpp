// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_BITMAPS_INCLUDED
#define CMAJOR_VIEW_BITMAPS_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/wing/Graphics.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

CMVIEW_API Bitmap* GetBitmap(const std::string& bitmapName);
CMVIEW_API void ClearBitmaps();
CMVIEW_API void InitBitmaps();
CMVIEW_API void DoneBitmaps();

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_BITMAPS_INCLUDED
